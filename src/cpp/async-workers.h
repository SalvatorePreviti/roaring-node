#ifndef ROARING_NODE_ASYNC_WORKERS_
#define ROARING_NODE_ASYNC_WORKERS_

#include "RoaringBitmap32.h"
#include "RoaringBitmap32-serialization.h"
#include "WorkerError.h"

uint32_t getCpusCount() {
  static uint32_t _cpusCountCache = 0;

  uint32_t result = _cpusCountCache;
  if (result != 0) {
    return result;
  }

  uv_cpu_info_t * tmp = nullptr;
  int count = 0;
  uv_cpu_info(&tmp, &count);
  if (tmp != nullptr) {
    uv_free_cpu_info(tmp, count);
  }
  result = count <= 0 ? 1 : (uint32_t)count;
  _cpusCountCache = result;
  return result;
}

class AsyncWorker {
 public:
  v8::Isolate * const isolate;
  AddonData * maybeAddonData;

  explicit AsyncWorker(v8::Isolate * isolate, AddonData * maybeAddonData) :
    isolate(isolate), maybeAddonData(maybeAddonData), _error(nullptr), _started(false), _completed(false) {
    _task.data = this;
  }

  virtual ~AsyncWorker() {}

  bool setCallback(v8::Local<v8::Value> callback) {
    if (callback.IsEmpty() || !callback->IsFunction()) return false;
    _callback.Reset(isolate, v8::Local<v8::Function>::Cast(callback));
    return true;
  }

  inline bool hasStarted() const { return this->_started; }

  inline bool hasError() const {
    if (this->_error.hasError()) {
      return true;
    }
    if (this->maybeAddonData != nullptr && !AddonData::isActive(this->maybeAddonData)) {
      return true;
    }
    return false;
  }

  inline bool isDown() const { return this->maybeAddonData != nullptr && !AddonData::isActive(this->maybeAddonData); }

  inline void setError(const WorkerError & error) {
    if (error.hasError() && !this->_error.hasError()) {
      this->_error = error;
    }
  }

  inline void clearError() { this->_error = WorkerError(); }

  static v8::Local<v8::Value> run(AsyncWorker * worker) {
    v8::EscapableHandleScope scope(worker->isolate);
    v8::Local<v8::Value> returnValue(v8::Undefined(worker->isolate));

    if (worker->_callback.IsEmpty()) {
      v8::Isolate * isolate = worker->isolate;
      v8::MaybeLocal<v8::Promise::Resolver> resolverMaybe = v8::Promise::Resolver::New(isolate->GetCurrentContext());

      if (resolverMaybe.IsEmpty()) {
        v8utils::throwTypeError(isolate, "Failed to create Promise");
        return returnValue;
      }

      v8::Local<v8::Promise::Resolver> resolver = resolverMaybe.ToLocalChecked();

      auto promise = resolver->GetPromise();
      if (promise.IsEmpty()) {
        worker->setError(WorkerError("Failed to create Promise"));
      } else {
        returnValue = promise;
        worker->_resolver.Reset(isolate, resolver);
      }
    }

    {
      v8::TryCatch tryCatch(worker->isolate);

      if (!worker->hasError()) {
        worker->before();
      }

      v8::Local<v8::Value> error;

      bool canContinue = true;

      if (tryCatch.HasCaught()) {
        canContinue = false;
        error = worker->_makeError(tryCatch.Exception());
        tryCatch.Reset();
        _resolveOrReject(worker, error);
      }

      if (canContinue && worker->hasError()) {
        canContinue = false;
        _resolveOrReject(worker, error);
      }

      if (canContinue && !worker->_start()) {
        if (tryCatch.HasCaught()) {
          error = worker->_makeError(tryCatch.Exception());
          tryCatch.Reset();
        }
        _resolveOrReject(worker, error);
      } else if (tryCatch.HasCaught()) {
        error = worker->_makeError(tryCatch.Exception());
        tryCatch.Reset();
        _resolveOrReject(worker, error);
      }
    }

    return scope.Escape(returnValue);
  }

 protected:
  // Called before the thread starts, in the main thread.
  virtual void before() {}

  // Called in a thread to execute the workload
  virtual void work() = 0;

  // Called after the thread completes without errors, in the main thread.
  virtual void done(v8::Local<v8::Value> & result) {}

  // Called after the thread completes, with or without errors, in the main thread.
  virtual void finally() {}

 private:
  uv_work_t _task{};
  WorkerError _error;
  bool _started;
  volatile bool _completed;
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> _callback;
  v8::Persistent<v8::Promise::Resolver, v8::CopyablePersistentTraits<v8::Promise::Resolver>> _resolver;

  virtual bool _start() {
    if (this->isDown()) {
      return false;
    }

    this->_started = true;
    if (
      uv_queue_work(node::GetCurrentEventLoop(v8::Isolate::GetCurrent()), &_task, AsyncWorker::_work, AsyncWorker::_done) !=
      0) {
      setError(WorkerError("Error starting async thread"));
      return false;
    }
    return true;
  }

  static void _resolveOrReject(AsyncWorker * worker, v8::Local<v8::Value> & error) {
    if (worker->_completed || worker->isDown()) {
      delete worker;
      return;
    }

    worker->_completed = true;

    v8::Isolate * isolate = worker->isolate;
    v8::HandleScope scope(isolate);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Value> result;

    if (!worker->_error.hasError() && error.IsEmpty()) {
      worker->done(result);
    }

    if (tryCatch.HasCaught()) {
      error = worker->_makeError(tryCatch.Exception());
      tryCatch.Reset();
    }

    worker->finally();

    if (tryCatch.HasCaught()) {
      error = worker->_makeError(tryCatch.Exception());
      tryCatch.Reset();
    }

    if (result.IsEmpty() && error.IsEmpty()) {
      worker->setError(WorkerError("Async operation failed"));
    }

    if (worker->hasError() && error.IsEmpty()) {
      error = worker->_error.newV8Error(isolate);
    }

    auto context = isolate->GetCurrentContext();

    if (worker->_resolver.IsEmpty()) {
      v8::Local<v8::Function> callback = worker->_callback.Get(isolate);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      if (!error.IsEmpty()) {
        v8::Local<v8::Value> argv[] = {error, v8::Undefined(isolate)};
        ignoreMaybeResult(callback->Call(context, context->Global(), 2, argv));
      } else {
        v8::Local<v8::Value> argv[] = {v8::Null(isolate), result};
        ignoreMaybeResult(callback->Call(context, context->Global(), 2, argv));
      }
      delete worker;
    } else {
      v8::Local<v8::Promise::Resolver> resolver = worker->_resolver.Get(isolate);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      delete worker;
      if (!error.IsEmpty()) {
        ignoreMaybeResult(resolver->Reject(context, error));
      } else if (!result.IsEmpty()) {
        ignoreMaybeResult(resolver->Resolve(context, result));
      } else {
        ignoreMaybeResult(resolver->Reject(context, v8::Undefined(isolate)));
      }
    }
  }

  static void _complete(AsyncWorker * worker) {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    v8::Isolate * isolate = worker->isolate;
    v8::Local<v8::Value> error;
    _resolveOrReject(worker, error);
  }

  static void _work(uv_work_t * request) {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto * worker = static_cast<AsyncWorker *>(request->data);
    if (worker && !worker->hasError()) {
      auto oldIsolate = thread_local_isolate;
      thread_local_isolate = worker->isolate;

      worker->work();

      thread_local_isolate = oldIsolate;
      std::atomic_thread_fence(std::memory_order_seq_cst);
    }
  }

  static void _done(uv_work_t * request, int status) {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto * worker = static_cast<AsyncWorker *>(request->data);

    auto oldIsolate = thread_local_isolate;
    thread_local_isolate = worker->isolate;

    if (status != 0) {
      worker->setError(WorkerError("Error executing async thread"));
    }
    _complete(worker);

    thread_local_isolate = oldIsolate;
  }

  v8::Local<v8::Value> _makeError(v8::Local<v8::Value> error) {
    if (error.IsEmpty() || error->IsNull() || error->IsUndefined()) {
      this->setError(WorkerError("Exception in async operation"));
      return {};
    }
    if (!error->IsObject()) {
      v8::MaybeLocal<v8::String> message = error->ToString(isolate->GetCurrentContext());
      if (message.IsEmpty()) {
        message = v8::String::NewFromUtf8Literal(isolate, "Operation failed", v8::NewStringType::kInternalized);
      }
      error = v8::Exception::Error(error.IsEmpty() ? v8::String::Empty(isolate) : message.ToLocalChecked());
    }
    return error;
  }

  friend class ParallelAsyncWorker;
};

class ParallelAsyncWorker : public AsyncWorker {
 public:
  uint32_t loopCount;
  uint32_t concurrency;

  explicit ParallelAsyncWorker(v8::Isolate * isolate, AddonData * maybeAddonData) :
    AsyncWorker(isolate, maybeAddonData),
    loopCount(0),
    concurrency(0),
    _tasks(nullptr),
    _pendingTasks(0),
    _currentIndex(0) {}

  virtual ~ParallelAsyncWorker() { gcaware_free(_tasks); }

 protected:
  void work() override {
    const uint32_t c = loopCount;
    for (uint32_t i = 0; i != c && !hasError() && !_completed; ++i) {
      parallelWork(i);
    }
  }

  virtual void parallelWork(uint32_t index) = 0;

 private:
  uv_work_t * _tasks;
  volatile int32_t _pendingTasks;
  volatile uint32_t _currentIndex;

  bool _start() override {
    if (this->isDown()) {
      return false;
    }

    if (concurrency == 0) {
      concurrency = getCpusCount();
    }

    uint32_t tasksCount = concurrency < loopCount ? concurrency : loopCount;

    if (tasksCount <= 1) {
      return AsyncWorker::_start();
    }

    uv_work_t * tasks = (uv_work_t *)gcaware_malloc(tasksCount * sizeof(uv_work_t));
    if (tasks == nullptr) {
      this->setError(WorkerError("Failed to allocate memory"));
      return false;
    }
    memset(tasks, 0, tasksCount * sizeof(uv_work_t));

    _tasks = tasks;

    for (uint32_t taskIndex = 0; taskIndex != tasksCount; ++taskIndex) {
      tasks[taskIndex].data = this;
    }

    for (uint32_t taskIndex = 0; taskIndex != tasksCount; ++taskIndex) {
      if (
        uv_queue_work(
          node::GetCurrentEventLoop(v8::Isolate::GetCurrent()),
          &tasks[taskIndex],
          ParallelAsyncWorker::_parallelWork,
          ParallelAsyncWorker::_parallelDone) != 0) {
        setError(WorkerError("Error starting async parallel task"));
        break;
      }
      ++_pendingTasks;
    }

    return _pendingTasks > 0;
  }

  static void _parallelWork(uv_work_t * request) {
    auto * worker = static_cast<ParallelAsyncWorker *>(request->data);

    if (worker) {
      if (worker->isDown()) {
        return;
      }

      auto oldIsolate = thread_local_isolate;
      thread_local_isolate = worker->isolate;
      uint32_t loopCount = worker->loopCount;
      while (!worker->hasError() && !worker->_completed) {
        const uint32_t prevIndex = worker->_currentIndex;
        const uint32_t index = atomicIncrement32(&worker->_currentIndex) - 1;
        if (index >= loopCount || index < prevIndex) {
          break;
        }
        worker->parallelWork(index);
      }
      thread_local_isolate = oldIsolate;
    }
  }

  static void _parallelDone(uv_work_t * request, int status) {
    auto * worker = static_cast<ParallelAsyncWorker *>(request->data);

    if (worker->_completed) {
      return;
    }

    if (--worker->_pendingTasks <= 0) {
      _complete(worker);
      return;
    }
  }
};

/////////////// ParallelAsyncWorker ///////////////

class RoaringBitmap32FactoryAsyncWorker : public AsyncWorker {
 public:
  volatile roaring_bitmap_t_ptr bitmap;

  explicit RoaringBitmap32FactoryAsyncWorker(v8::Isolate * isolate, AddonData * addonData) :
    AsyncWorker(isolate, addonData), bitmap(nullptr) {}

  virtual ~RoaringBitmap32FactoryAsyncWorker() {
    if (this->bitmap != nullptr) {
      roaring_bitmap_free(this->bitmap);
      this->bitmap = nullptr;
    }
  }

 protected:
  void done(v8::Local<v8::Value> & result) override {
    if (this->bitmap == nullptr || !AddonData::isActive(this->maybeAddonData)) {
      return this->setError(WorkerError("Error deserializing roaring bitmap"));
    }

    v8::Local<v8::Function> cons = this->maybeAddonData->RoaringBitmap32_constructor.Get(this->isolate);

    v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);

    if (!resultMaybe.ToLocal(&result)) {
      return this->setError(WorkerError("Error instantiating roaring bitmap"));
    }

    RoaringBitmap32 * unwrapped = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
    if (unwrapped == nullptr) {
      return this->setError(WorkerError(ERROR_INVALID_OBJECT));
    }

    unwrapped->replaceBitmapInstance(this->isolate, this->bitmap);
    this->bitmap = nullptr;
  }
};

class ToUint32ArrayAsyncWorker final : public AsyncWorker {
 public:
  const v8::FunctionCallbackInfo<v8::Value> & info;
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bitmapPersistent;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> bufferPersistent;
  RoaringBitmap32 * bitmap = nullptr;
  v8utils::TypedArrayContent<uint32_t> inputContent;
  uint32_t * volatile allocatedBuffer = nullptr;
  size_t volatile outputSize = 0;
  size_t maxSize = std::numeric_limits<size_t>::max();
  bool hasInput = false;

  explicit ToUint32ArrayAsyncWorker(const v8::FunctionCallbackInfo<v8::Value> & info, AddonData * maybeAddonData) :
    AsyncWorker(info.GetIsolate(), maybeAddonData), info(info) {
    gcaware_addAllocatedMemory(sizeof(ToUint32ArrayAsyncWorker));
  }

  ~ToUint32ArrayAsyncWorker() {
    if (this->allocatedBuffer) {
      bare_aligned_free(this->allocatedBuffer);
    }
    gcaware_removeAllocatedMemory(sizeof(ToUint32ArrayAsyncWorker));
  }

 protected:
  // Called before the thread starts, in the main thread.
  void before() final {
    v8::Isolate * isolate = info.GetIsolate();

    RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
    if (self == nullptr) {
      return this->setError(WorkerError(ERROR_INVALID_OBJECT));
    }
    if (this->maybeAddonData == nullptr) {
      this->maybeAddonData = self->addonData;
    }

    if (info.Length() >= 1 && !info[0]->IsUndefined()) {
      if (info[0]->IsNumber()) {
        double maxSizeDouble;
        if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&maxSizeDouble) || std::isnan(maxSizeDouble)) {
          return v8utils::throwError(
            isolate, "RoaringBitmap32::toUint32ArrayAsync - argument must be a valid integer number");
        }
        this->maxSize = maxSizeDouble <= 0 ? 0 : (maxSizeDouble > 0xfffffffff ? 0xfffffffff : (size_t)maxSizeDouble);
      } else {
        v8::Local<v8::Object> obj;

        if (
          !argumentIsValidUint32ArrayOutput(info[0]) || !info[0]->ToObject(isolate->GetCurrentContext()).ToLocal(&obj) ||
          !this->inputContent.set(isolate, obj)) {
          return v8utils::throwError(
            isolate, "RoaringBitmap32::toUint32ArrayAsync - argument must be a UInt32Array, Int32Array or ArrayBuffer");
        }

        this->bufferPersistent.Reset(isolate, obj);
        this->hasInput = true;
      }
    }

    this->bitmap = self;
    this->bitmap->beginFreeze();
  }

  void work() final {
    auto size = this->bitmap->getSize();
    if (size == 0) {
      return;
    }

    if (this->hasInput) {
      if (size > this->inputContent.length) {
        this->outputSize = this->inputContent.length;
        roaring_bitmap_range_uint32_array(this->bitmap->roaring, 0, this->outputSize, this->inputContent.data);
      } else {
        this->outputSize = size;
        roaring_bitmap_to_uint32_array(this->bitmap->roaring, this->inputContent.data);
      }
      return;
    }

    auto maxSize = this->maxSize;
    if (maxSize > size) {
      maxSize = size;
    }

    // Allocate a new buffer
    this->allocatedBuffer = (uint32_t *)bare_aligned_malloc(32, size * sizeof(uint32_t));
    if (!this->allocatedBuffer) {
      return this->setError(WorkerError("RoaringBitmap32::toUint32ArrayAsync - failed to allocate memory"));
    }

    if (maxSize < size) {
      roaring_bitmap_range_uint32_array(this->bitmap->roaring, 0, maxSize, this->allocatedBuffer);
    } else {
      roaring_bitmap_to_uint32_array(this->bitmap->roaring, this->allocatedBuffer);
    }

    this->outputSize = maxSize;
  }

  void finally() final {
    if (this->bitmap) {
      this->bitmap->endFreeze();
    }
  }

  void done(v8::Local<v8::Value> & result) final {
    uint32_t * allocatedBuffer = this->allocatedBuffer;

    if (this->hasInput) {
      if (!v8utils::v8ValueToUint32ArrayWithLimit(isolate, this->bufferPersistent.Get(isolate), this->outputSize, result)) {
        return this->setError(WorkerError("RoaringBitmap32::toUint32ArrayAsync - failed to create a UInt32Array range"));
      }
      return;
    }

    if (allocatedBuffer && this->outputSize != 0) {
      // Create a new buffer using the allocated memory
      v8::MaybeLocal<v8::Object> nodeBufferMaybeLocal = node::Buffer::New(
        isolate, (char *)allocatedBuffer, this->outputSize * sizeof(uint32_t), bare_aligned_free_callback, allocatedBuffer);
      if (!nodeBufferMaybeLocal.IsEmpty()) {
        this->allocatedBuffer = nullptr;
      }

      v8::Local<v8::Object> nodeBufferObject;
      if (!nodeBufferMaybeLocal.ToLocal(&nodeBufferObject)) {
        return this->setError(WorkerError("RoaringBitmap32::toUint32ArrayAsync - failed to create a new buffer"));
      }

      v8::Local<v8::Uint8Array> nodeBuffer = nodeBufferObject.As<v8::Uint8Array>();
      if (nodeBuffer.IsEmpty()) {
        return this->setError(WorkerError("RoaringBitmap32::toUint32ArrayAsync - failed to create a new buffer"));
      }
      result = v8::Uint32Array::New(nodeBuffer->Buffer(), 0, this->outputSize);
      if (result.IsEmpty()) {
        return this->setError(WorkerError("RoaringBitmap32::toUint32ArrayAsync - failed to create a new buffer"));
      }
      return;
    }

    auto arrayBuffer = v8::ArrayBuffer::New(isolate, 0);
    if (arrayBuffer.IsEmpty()) {
      return this->setError(WorkerError("RoaringBitmap32::toUint32ArrayAsync - failed to create an empty ArrayBuffer"));
    }
    result = v8::Uint32Array::New(arrayBuffer, 0, 0);
    if (result.IsEmpty()) {
      return this->setError(WorkerError("RoaringBitmap32::toUint32ArrayAsync - failed to create an empty ArrayBuffer"));
    }
  }
};

class SerializeWorker final : public AsyncWorker {
 public:
  const v8::FunctionCallbackInfo<v8::Value> & info;
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bitmapPersistent;
  RoaringBitmapSerializer serializer;

  explicit SerializeWorker(const v8::FunctionCallbackInfo<v8::Value> & info, AddonData * maybeAddonData) :
    AsyncWorker(info.GetIsolate(), maybeAddonData), info(info) {
    gcaware_addAllocatedMemory(sizeof(SerializeWorker));
  }

  virtual ~SerializeWorker() { gcaware_removeAllocatedMemory(sizeof(SerializeWorker)); }

 protected:
  // Called before the thread starts, in the main thread.
  void before() final {
    this->serializer.parseArguments(this->info);
    if (this->serializer.self) {
      if (this->maybeAddonData == nullptr) {
        this->maybeAddonData = this->serializer.self->addonData;
      }
      this->bitmapPersistent.Reset(isolate, this->info.Holder());
      this->serializer.self->beginFreeze();
    }
  }

  void work() final {
    if (this->serializer.self) {
      this->setError(this->serializer.serialize());
    }
  }

  void finally() final {
    if (this->serializer.self) {
      this->serializer.self->endFreeze();
    }
  }

  void done(v8::Local<v8::Value> & result) final { this->serializer.done(this->isolate, result); }
};

class SerializeFileWorker final : public AsyncWorker {
 public:
  const v8::FunctionCallbackInfo<v8::Value> & info;
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bitmapPersistent;
  RoaringBitmapFileSerializer serializer;

  explicit SerializeFileWorker(const v8::FunctionCallbackInfo<v8::Value> & info, AddonData * maybeAddonData) :
    AsyncWorker(info.GetIsolate(), maybeAddonData), info(info) {
    gcaware_addAllocatedMemory(sizeof(SerializeFileWorker));
  }

  virtual ~SerializeFileWorker() { gcaware_removeAllocatedMemory(sizeof(SerializeFileWorker)); }

 protected:
  // Called before the thread starts, in the main thread.
  void before() final {
    this->serializer.parseArguments(this->info);
    if (this->serializer.self) {
      if (this->maybeAddonData == nullptr) {
        this->maybeAddonData = this->serializer.self->addonData;
      }
      this->bitmapPersistent.Reset(isolate, this->info.Holder());
      this->serializer.self->beginFreeze();
    }
  }

  void work() final {
    if (this->serializer.self) {
      this->setError(this->serializer.serialize());
    }
  }

  void finally() final {
    if (this->serializer.self) {
      this->serializer.self->endFreeze();
    }
  }

  void done(v8::Local<v8::Value> & result) final {
    if (!this->bitmapPersistent.IsEmpty()) {
      result = this->bitmapPersistent.Get(this->isolate);
    }
  }
};

class DeserializeWorker final : public AsyncWorker {
 public:
  RoaringBitmapDeserializer deserializer;
  const v8::FunctionCallbackInfo<v8::Value> & info;

  explicit DeserializeWorker(const v8::FunctionCallbackInfo<v8::Value> & info, AddonData * addonData) :
    AsyncWorker(info.GetIsolate(), addonData), info(info) {
    gcaware_addAllocatedMemory(sizeof(DeserializeWorker));
  }

  virtual ~DeserializeWorker() { gcaware_removeAllocatedMemory(sizeof(DeserializeWorker)); }

 protected:
  void before() final {
    this->setError(this->deserializer.parseArguments(this->info, false));
    if (this->maybeAddonData == nullptr) {
      if (this->deserializer.targetBitmap != nullptr) {
        this->maybeAddonData = this->deserializer.targetBitmap->addonData;
      }
      if (this->maybeAddonData == nullptr && !this->hasError()) {
        this->setError(WorkerError("RoaringBitmap32 deserialization failed to get the addon data"));
      }
    }
  }

  void work() final { this->setError(this->deserializer.deserialize()); }

  void done(v8::Local<v8::Value> & result) final {
    v8::Isolate * isolate = this->isolate;

    if (!AddonData::isActive(this->maybeAddonData)) {
      return this->setError(WorkerError(ERROR_INVALID_OBJECT));
    }

    v8::Local<v8::Function> cons = this->maybeAddonData->RoaringBitmap32_constructor.Get(isolate);

    if (!cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr).ToLocal(&result)) {
      return this->setError(WorkerError("RoaringBitmap32 deserialization failed to create a new instance"));
    }

    RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
    if (self == nullptr) {
      return this->setError(WorkerError(ERROR_INVALID_OBJECT));
    }

    self->replaceBitmapInstance(isolate, nullptr);

    this->deserializer.finalizeTargetBitmap(self);
  }
};

/**
 * Same as DeserializeWorker but it uses memory mapped files to deserialize the bitmaps.
 */
class DeserializeFileWorker final : public AsyncWorker {
 public:
  RoaringBitmapFileDeserializer deserializer;
  const v8::FunctionCallbackInfo<v8::Value> & info;

  explicit DeserializeFileWorker(const v8::FunctionCallbackInfo<v8::Value> & info, AddonData * addonData) :
    AsyncWorker(info.GetIsolate(), addonData), info(info) {
    gcaware_addAllocatedMemory(sizeof(DeserializeFileWorker));
  }

  virtual ~DeserializeFileWorker() { gcaware_removeAllocatedMemory(sizeof(DeserializeFileWorker)); }

 protected:
  void before() final { this->setError(this->deserializer.parseArguments(this->info)); }

  void work() final { this->setError(this->deserializer.deserialize()); }

  void done(v8::Local<v8::Value> & result) {
    v8::Isolate * isolate = this->isolate;

    if (!AddonData::isActive(this->maybeAddonData)) {
      return this->setError(WorkerError(ERROR_INVALID_OBJECT));
    }

    v8::Local<v8::Function> cons = this->maybeAddonData->RoaringBitmap32_constructor.Get(isolate);

    if (!cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr).ToLocal(&result)) {
      return this->setError(WorkerError("RoaringBitmap32 deserialization failed to create a new instance"));
    }

    RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
    if (self == nullptr) {
      return this->setError(WorkerError(ERROR_INVALID_OBJECT));
    }

    self->replaceBitmapInstance(isolate, nullptr);

    this->deserializer.finalizeTargetBitmap(self);
  }
};

class DeserializeParallelWorker : public ParallelAsyncWorker {
 public:
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bufferPersistent;

  RoaringBitmapDeserializer * items;

  explicit DeserializeParallelWorker(v8::Isolate * isolate, AddonData * maybeAddonData) :
    ParallelAsyncWorker(isolate, maybeAddonData), items(nullptr) {}

  virtual ~DeserializeParallelWorker() {
    if (items) {
      delete[] items;
    }
  }

 protected:
  virtual void parallelWork(uint32_t index) {
    RoaringBitmapDeserializer & item = items[index];
    const WorkerError error = item.deserialize();
    if (error.hasError()) {
      this->setError(error);
    }
  }

  void done(v8::Local<v8::Value> & result) final {
    if (!AddonData::isActive(this->maybeAddonData)) {
      return this->setError(WorkerError(ERROR_INVALID_OBJECT));
    }

    v8::Local<v8::Function> cons = this->maybeAddonData->RoaringBitmap32_constructor.Get(isolate);

    const uint32_t itemsCount = this->loopCount;
    RoaringBitmapDeserializer * items = this->items;

    v8::MaybeLocal<v8::Array> resultArrayMaybe = v8::Array::New(isolate, itemsCount);
    v8::Local<v8::Array> resultArray;
    if (!resultArrayMaybe.ToLocal(&resultArray)) {
      return this->setError(WorkerError("RoaringBitmap32 deserialization failed to create a new array"));
    }

    v8::Local<v8::Context> currentContext = isolate->GetCurrentContext();

    for (uint32_t i = 0; i != itemsCount; ++i) {
      v8::MaybeLocal<v8::Object> instanceMaybe = cons->NewInstance(currentContext, 0, nullptr);
      v8::Local<v8::Object> instance;
      if (!instanceMaybe.ToLocal(&instance)) {
        return this->setError(WorkerError("RoaringBitmap32 deserialization failed to create a new instance"));
      }

      RoaringBitmap32 * unwrapped = ObjectWrap::TryUnwrap<RoaringBitmap32>(instance, isolate);
      if (unwrapped == nullptr) {
        return this->setError(WorkerError(ERROR_INVALID_OBJECT));
      }

      RoaringBitmapDeserializer & item = items[i];
      item.finalizeTargetBitmap(unwrapped);
      ignoreMaybeResult(resultArray->Set(currentContext, i, instance));
    }

    result = resultArray;
  }
};

class FromArrayAsyncWorker : public RoaringBitmap32FactoryAsyncWorker {
 public:
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> argPersistent;
  v8utils::TypedArrayContent<uint32_t> buffer;

  explicit FromArrayAsyncWorker(v8::Isolate * isolate, AddonData * addonData) :
    RoaringBitmap32FactoryAsyncWorker(isolate, addonData) {
    gcaware_addAllocatedMemory(sizeof(FromArrayAsyncWorker));
  }

  virtual ~FromArrayAsyncWorker() { gcaware_removeAllocatedMemory(sizeof(FromArrayAsyncWorker)); }

 protected:
  void work() final {
    bitmap = roaring_bitmap_create_with_capacity(buffer.length);
    if (bitmap == nullptr) {
      this->setError(WorkerError("Failed to allocate roaring bitmap"));
      return;
    }
    roaring_bitmap_add_many(bitmap, buffer.length, buffer.data);
    roaring_bitmap_run_optimize(bitmap);
    roaring_bitmap_shrink_to_fit(bitmap);
  }
};

#endif  // ROARING_NODE_ASYNC_WORKERS_

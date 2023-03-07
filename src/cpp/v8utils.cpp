#include "v8utils.h"
#include "memory.h"

/////////////// v8utils ///////////////

namespace v8utils {

  static uint32_t _cpusCountCache = 0;

  uint32_t getCpusCount() {
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

  // Creates a new Error from string
  v8::Local<v8::Value> createError(v8::Isolate * isolate, const char * message) {
    v8::EscapableHandleScope scope(isolate);
    auto msg = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
    v8::Local<v8::String> msgLocal;
    if (msg.ToLocal(&msgLocal)) {
      return scope.Escape(v8::Exception::Error(msgLocal));
    }
    return scope.Escape(
      v8::Exception::Error(NEW_LITERAL_V8_STRING(isolate, "Operation failed", v8::NewStringType::kInternalized)));
  }

  void throwError(v8::Isolate * isolate, const char * message) {
    v8::HandleScope scope(isolate);
    if (message != nullptr && message[0] != '\0') {
      auto msg = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
      v8::Local<v8::String> msgLocal;
      if (msg.ToLocal(&msgLocal)) {
        isolate->ThrowException(v8::Exception::Error(msgLocal));
        return;
      }
    }
    isolate->ThrowException(
      v8::Exception::Error(NEW_LITERAL_V8_STRING(isolate, "Operation failed", v8::NewStringType::kInternalized)));
  }

  void throwTypeError(v8::Isolate * isolate, const char * message) {
    v8::HandleScope scope(isolate);
    if (message != nullptr && message[0] != '\0') {
      auto msg = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
      v8::Local<v8::String> msgLocal;
      if (msg.ToLocal(&msgLocal)) {
        isolate->ThrowException(v8::Exception::TypeError(msgLocal));
        return;
      }
    }
    isolate->ThrowException(
      v8::Exception::TypeError(NEW_LITERAL_V8_STRING(isolate, "Operation failed", v8::NewStringType::kInternalized)));
  }

  void throwTypeError(v8::Isolate * isolate, const char * context, const char * message) {
    v8::HandleScope scope(isolate);
    auto a = v8::String::NewFromUtf8(isolate, context, v8::NewStringType::kInternalized);
    auto b = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
#if NODE_MAJOR_VERSION > 10
    auto msg = a.IsEmpty() ? b : b.IsEmpty() ? a : v8::String::Concat(isolate, a.ToLocalChecked(), b.ToLocalChecked());
#else
    auto msg = a.IsEmpty() ? b : b.IsEmpty() ? a : v8::String::Concat(a.ToLocalChecked(), b.ToLocalChecked());
#endif
    isolate->ThrowException(v8::Exception::TypeError(msg.IsEmpty() ? v8::String::Empty(isolate) : msg.ToLocalChecked()));
  }

  static bool _bufferFromArrayBuffer(
    v8::Isolate * isolate, v8::Local<v8::Value> buffer, size_t offset, size_t length, v8::Local<v8::Value> & result) {
    if (buffer.IsEmpty()) {
      return false;
    }
#if NODE_MAJOR_VERSION > 12
    auto buf = buffer.As<v8::ArrayBuffer>();
    return !buf.IsEmpty() && node::Buffer::New(isolate, buf, offset, length).ToLocal(&result);
#else
    v8::Local<v8::Value> argv[] = {
      buffer, v8::Integer::NewFromUnsigned(isolate, offset), v8::Integer::NewFromUnsigned(isolate, length)};
    return JSTypes::Buffer_from.Get(isolate)
      ->Call(isolate->GetCurrentContext(), JSTypes::Uint32Array.Get(isolate), 3, argv)
      .ToLocal(&result);
#endif
  }

  bool v8ValueToBufferWithLimit(
    v8::Isolate * isolate, v8::MaybeLocal<v8::Value> value, size_t length, v8::Local<v8::Value> & result) {
    v8::Local<v8::Value> localValue;
    if (value.ToLocal(&localValue) && !localValue.IsEmpty()) {
      if (localValue->IsUint8Array()) {
        v8::Local<v8::Uint8Array> array = localValue.As<v8::Uint8Array>();
        if (!array.IsEmpty()) {
          if (node::Buffer::HasInstance(localValue) && node::Buffer::Length(localValue) == length) {
            result = array;
            return true;
          }
          if (array->ByteLength() >= length) {
            return _bufferFromArrayBuffer(isolate, array->Buffer(), array->ByteOffset(), length, result);
          }
        }
        return false;
      }
      if (localValue->IsTypedArray()) {
        auto array = localValue.As<v8::TypedArray>();
        if (!array.IsEmpty() && array->ByteLength() >= length) {
          return _bufferFromArrayBuffer(isolate, array->Buffer(), array->ByteOffset(), length, result);
        }
        return false;
      }
      if (localValue->IsArrayBufferView()) {
        auto array = localValue.As<v8::ArrayBufferView>();
        if (!array.IsEmpty() && array->ByteLength() >= length) {
          return _bufferFromArrayBuffer(isolate, array->Buffer(), array->ByteOffset(), length, result);
        }
        return false;
      }
      if (localValue->IsArrayBuffer()) {
        auto array = localValue.As<v8::ArrayBuffer>();
        if (!array.IsEmpty() && array->ByteLength() >= length) {
          return _bufferFromArrayBuffer(isolate, array, 0, length, result);
        }
        return false;
      }
    }
    return false;
  }

  bool v8ValueToUint32ArrayWithLimit(
    v8::Isolate * isolate, v8::MaybeLocal<v8::Value> value, size_t length, v8::Local<v8::Value> & result) {
    v8::Local<v8::Value> localValue;
    if (!value.ToLocal(&localValue)) {
      return {};
    }
    if (localValue->IsUint32Array()) {
      auto array = localValue.As<v8::Uint32Array>();
      if (!array.IsEmpty()) {
        auto arrayLength = array->Length();
        if (arrayLength == length) {
          result = array;
          return true;
        }
        if (arrayLength > length) {
          result = v8::Uint32Array::New(array->Buffer(), array->ByteOffset(), length);
          return !result.IsEmpty();
        }
      }
      return false;
    }
    if (localValue->IsTypedArray()) {
      auto array = localValue.As<v8::TypedArray>();
      if (array->ByteLength() >= length * sizeof(uint32_t)) {
        result = v8::Uint32Array::New(array->Buffer(), array->ByteOffset(), length);
        return !result.IsEmpty();
      }
      return false;
    }
    if (localValue->IsArrayBufferView()) {
      auto array = localValue.As<v8::ArrayBufferView>();
      if (array->ByteLength() >= length * sizeof(uint32_t)) {
        result = v8::Uint32Array::New(array->Buffer(), array->ByteOffset(), length);
        return !result.IsEmpty();
      }
      return false;
    }
    if (localValue->IsArrayBuffer()) {
      auto array = localValue.As<v8::ArrayBuffer>();
      if (array->ByteLength() >= length * sizeof(uint32_t)) {
        result = v8::Uint32Array::New(array, 0, length);
        return !result.IsEmpty();
      }
      return false;
    }
    return false;
  }

  /////////////// AsyncWorker ///////////////

  AsyncWorker::AsyncWorker(v8::Isolate * isolate) : isolate(isolate), _error(nullptr), _started(false), _completed(false) {
    _task.data = this;
  }

  AsyncWorker::~AsyncWorker() {}

  bool AsyncWorker::setCallback(v8::Local<v8::Value> callback) {
    if (callback.IsEmpty() || !callback->IsFunction()) return false;
    _callback.Reset(isolate, v8::Local<v8::Function>::Cast(callback));
    return true;
  }

  bool AsyncWorker::_start() {
    this->_started = true;
    if (uv_queue_work(uv_default_loop(), &_task, AsyncWorker::_work, AsyncWorker::_done) != 0) {
      setError("Error starting async thread");
      return false;
    }
    return true;
  }

  v8::Local<v8::Value> AsyncWorker::_makeError(v8::Local<v8::Value> error) {
    if (error.IsEmpty() || error->IsNull() || error->IsUndefined()) {
      this->setError("Exception in async operation");
      return {};
    }
    if (!error->IsObject()) {
      v8::MaybeLocal<v8::String> message = error->ToString(isolate->GetCurrentContext());
      if (message.IsEmpty()) {
        message = NEW_LITERAL_V8_STRING(isolate, "Operation failed", v8::NewStringType::kInternalized);
      }
      error = v8::Exception::Error(error.IsEmpty() ? v8::String::Empty(isolate) : message.ToLocalChecked());
    }
    return error;
  }

  v8::Local<v8::Value> AsyncWorker::run(AsyncWorker * worker) {
    v8::EscapableHandleScope scope(worker->isolate);
    v8::Local<v8::Value> returnValue;

    if (worker->_callback.IsEmpty()) {
      v8::Isolate * isolate = worker->isolate;
      v8::MaybeLocal<v8::Promise::Resolver> resolverMaybe = v8::Promise::Resolver::New(isolate->GetCurrentContext());

      if (resolverMaybe.IsEmpty()) {
        v8utils::throwTypeError(isolate, "Failed to create Promise");
        return returnValue;
      }

      v8::Local<v8::Promise::Resolver> resolver = resolverMaybe.ToLocalChecked();

      returnValue = resolver->GetPromise();
      if (returnValue.IsEmpty()) {
        worker->setError("Failed to create Promise");
      } else {
        worker->_resolver.Reset(isolate, resolver);
      }
    }

    v8::TryCatch tryCatch(worker->isolate);
    worker->before();

    v8::Local<v8::Value> error;

    if (worker->hasError()) {
      _resolveOrReject(worker, error);
    } else if (tryCatch.HasCaught()) {
      error = worker->_makeError(tryCatch.Exception());
      _resolveOrReject(worker, error);
    } else if (!worker->_start()) {
      if (tryCatch.HasCaught()) {
        error = worker->_makeError(tryCatch.Exception());
      }
      _resolveOrReject(worker, error);
    }

    if (returnValue.IsEmpty()) {
      returnValue = v8::Undefined(worker->isolate);
    }

    return scope.Escape(returnValue);
  }

  void AsyncWorker::before() {}

  void AsyncWorker::done(v8::Local<v8::Value> & result) {}

  void AsyncWorker::finally() {}

  void AsyncWorker::_work(uv_work_t * request) {
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

  void AsyncWorker::_done(uv_work_t * request, int status) {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto * worker = static_cast<AsyncWorker *>(request->data);
    if (status != 0) {
      worker->setError("Error executing async thread");
    }
    _complete(worker);
  }

  void AsyncWorker::_resolveOrReject(AsyncWorker * worker, v8::Local<v8::Value> & error) {
    if (worker->_completed) {
      return;
    }

    worker->_completed = true;

    v8::Isolate * isolate = worker->isolate;
    v8::HandleScope scope(isolate);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Value> result;

    if (worker->_error == nullptr && error.IsEmpty()) {
      worker->done(result);

      if (tryCatch.HasCaught()) {
        error = worker->_makeError(tryCatch.Exception());
        tryCatch.Reset();
      }
    }

    worker->finally();

    if (tryCatch.HasCaught()) {
      error = worker->_makeError(tryCatch.Exception());
      tryCatch.Reset();
    }

    if (result.IsEmpty() && error.IsEmpty()) {
      worker->setError("Async operation failed");
    }

    if (worker->hasError() && error.IsEmpty()) {
      v8::MaybeLocal<v8::String> message =
        v8::String::NewFromUtf8(isolate, worker->_error, v8::NewStringType::kInternalized);
      error = v8::Exception::Error(message.IsEmpty() ? v8::String::Empty(isolate) : message.ToLocalChecked());
    }

    auto context = isolate->GetCurrentContext();
    if (worker->_resolver.IsEmpty()) {
      v8::Local<v8::Function> callback = worker->_callback.Get(isolate);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      delete worker;
      if (!error.IsEmpty()) {
        v8::Local<v8::Value> argv[] = {error, v8::Undefined(isolate)};
        v8utils::ignoreMaybeResult(callback->Call(context, context->Global(), 2, argv));
      } else {
        v8::Local<v8::Value> argv[] = {v8::Null(isolate), result};
        v8utils::ignoreMaybeResult(callback->Call(context, context->Global(), 2, argv));
      }
    } else {
      v8::Local<v8::Promise::Resolver> resolver = worker->_resolver.Get(isolate);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      delete worker;
      if (!error.IsEmpty()) {
        v8utils::ignoreMaybeResult(resolver->Reject(context, error));
      } else if (!result.IsEmpty()) {
        v8utils::ignoreMaybeResult(resolver->Resolve(context, result));
      } else {
        v8utils::ignoreMaybeResult(resolver->Reject(context, v8::Undefined(isolate)));
      }
    }
  }

  void AsyncWorker::_complete(AsyncWorker * worker) {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    v8::Isolate * isolate = worker->isolate;
    v8::Local<v8::Value> error;
    _resolveOrReject(worker, error);
#if NODE_MAJOR_VERSION > 13
    isolate->PerformMicrotaskCheckpoint();
#else
    isolate->RunMicrotasks();
#endif
  }

  /////////////// ParallelAsyncWorker ///////////////

  ParallelAsyncWorker::ParallelAsyncWorker(v8::Isolate * isolate) :
    AsyncWorker(isolate), loopCount(0), concurrency(0), _tasks(nullptr), _pendingTasks(0), _currentIndex(0) {}

  ParallelAsyncWorker::~ParallelAsyncWorker() { gcaware_free(_tasks); }

  void ParallelAsyncWorker::work() {
    const uint32_t c = loopCount;
    for (uint32_t i = 0; i != c && !hasError() && !_completed; ++i) {
      parallelWork(i);
    }
  }

  bool ParallelAsyncWorker::_start() {
    if (concurrency == 0) {
      concurrency = getCpusCount();
    }

    uint32_t tasksCount = concurrency < loopCount ? concurrency : loopCount;

    if (tasksCount <= 1) {
      return AsyncWorker::_start();
    }

    uv_work_t * tasks = (uv_work_t *)gcaware_malloc(tasksCount * sizeof(uv_work_t));
    if (tasks == nullptr) {
      this->setError("Failed to allocate memory");
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
          uv_default_loop(), &tasks[taskIndex], ParallelAsyncWorker::_parallelWork, ParallelAsyncWorker::_parallelDone) !=
        0) {
        setError("Error starting async parallel task");
        break;
      }
      ++_pendingTasks;
    }

    return _pendingTasks > 0;
  }

  void ParallelAsyncWorker::_parallelWork(uv_work_t * request) {
    auto * worker = static_cast<ParallelAsyncWorker *>(request->data);

    if (worker) {
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

  void ParallelAsyncWorker::_parallelDone(uv_work_t * request, int /*status*/) {
    auto * worker = static_cast<ParallelAsyncWorker *>(request->data);

    if (worker->_completed) {
#if NODE_MAJOR_VERSION > 13
      worker->isolate->PerformMicrotaskCheckpoint();
#else
      worker->isolate->RunMicrotasks();
#endif
      return;
    }

    if (--worker->_pendingTasks <= 0) {
      _complete(worker);
      return;
    }

#if NODE_MAJOR_VERSION > 13
    worker->isolate->PerformMicrotaskCheckpoint();
#else
    worker->isolate->RunMicrotasks();
#endif
  }

}  // namespace v8utils

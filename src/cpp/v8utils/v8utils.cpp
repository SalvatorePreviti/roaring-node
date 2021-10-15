#include "v8utils.h"

#ifdef _MSC_VER
#  define atomicIncrement32(ptr) InterlockedIncrement(ptr)
#  define atomicDecrement32(ptr) InterlockedDecrement(ptr)
#else
#  define atomicIncrement32(ptr) __sync_add_and_fetch(ptr, 1)
#  define atomicDecrement32(ptr) __sync_sub_and_fetch(ptr, 1)
#endif

/////////////// JSTypes ///////////////

v8::Eternal<v8::Object> JSTypes::Uint32Array;
v8::Eternal<v8::Function> JSTypes::Uint32Array_ctor;
v8::Eternal<v8::Function> JSTypes::Uint32Array_from;

v8::Eternal<v8::Object> JSTypes::Buffer;
v8::Eternal<v8::Function> JSTypes::Buffer_allocUnsafe;

v8::Eternal<v8::Object> JSTypes::Array;
v8::Eternal<v8::Function> JSTypes::Array_from;

v8::Eternal<v8::Object> JSTypes::Set;
v8::Eternal<v8::Function> JSTypes::Set_ctor;

void JSTypes::initJSTypes(v8::Isolate * isolate, const v8::Local<v8::Object> & global) {
  v8::HandleScope scope(isolate);
  auto context = isolate->GetCurrentContext();

  auto uint32Array = global->Get(context, v8::String::NewFromUtf8(isolate, "Uint32Array", v8::NewStringType::kInternalized).ToLocalChecked())
                         .ToLocalChecked()
                         ->ToObject(context)
                         .ToLocalChecked();

  JSTypes::Uint32Array.Set(isolate, uint32Array);
  JSTypes::Uint32Array_ctor.Set(isolate, v8::Local<v8::Function>::Cast(uint32Array));
  JSTypes::Uint32Array_from.Set(isolate,
      v8::Local<v8::Function>::Cast(
          uint32Array->Get(context, v8::String::NewFromUtf8(isolate, "from", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()));

  auto buffer = global->Get(context, v8::String::NewFromUtf8(isolate, "Buffer", v8::NewStringType::kInternalized).ToLocalChecked())
                    .ToLocalChecked()
                    ->ToObject(context)
                    .ToLocalChecked();
  JSTypes::Buffer.Set(isolate, buffer);
  JSTypes::Buffer_allocUnsafe.Set(isolate,
      v8::Local<v8::Function>::Cast(
          buffer->Get(context, v8::String::NewFromUtf8(isolate, "allocUnsafe", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()));

  auto array = global->Get(context, v8::String::NewFromUtf8(isolate, "Array", v8::NewStringType::kInternalized).ToLocalChecked())
                   .ToLocalChecked()
                   ->ToObject(context)
                   .ToLocalChecked();
  JSTypes::Array.Set(isolate, array);
  JSTypes::Array_from.Set(isolate,
      v8::Local<v8::Function>::Cast(
          array->Get(context, v8::String::NewFromUtf8(isolate, "from", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()));

  auto set = global->Get(context, v8::String::NewFromUtf8(isolate, "Set", v8::NewStringType::kInternalized).ToLocalChecked())
                 .ToLocalChecked()
                 ->ToObject(context)
                 .ToLocalChecked();
  JSTypes::Set.Set(isolate, set);
  JSTypes::Set_ctor.Set(isolate, v8::Local<v8::Function>::Cast(set));
}

v8::Local<v8::Value> JSTypes::bufferAllocUnsafe(v8::Isolate * isolate, size_t size) {
  v8::Local<v8::Value> argv[] = {v8::Number::New(isolate, (double)size)};
  auto x = JSTypes::Buffer_allocUnsafe.Get(isolate)->Call(isolate->GetCurrentContext(), JSTypes::Uint32Array.Get(isolate), 1, argv);
  if (x.IsEmpty()) {
    v8utils::throwTypeError(isolate, "Cannot allocate Buffer");
    return v8::Null(isolate);
  }
  return x.ToLocalChecked();
}

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
    if (tmp) {
      uv_free_cpu_info(tmp, count);
    }
    result = count <= 0 ? 1 : (uint32_t)count;
    _cpusCountCache = result;
    return result;
  }

  void throwError(v8::Isolate * isolate, const char * message) {
    v8::HandleScope scope(isolate);
    auto msg = v8::String::NewFromUtf8(isolate, message ? message : "Operation failed", v8::NewStringType::kNormal);
    isolate->ThrowException(v8::Exception::Error(msg.IsEmpty() ? v8::String::Empty(isolate) : msg.ToLocalChecked()));
  }

  void throwTypeError(v8::Isolate * isolate, const char * message) {
    v8::HandleScope scope(isolate);
    auto msg = v8::String::NewFromUtf8(isolate, message ? message : "Operation failed", v8::NewStringType::kNormal);
    isolate->ThrowException(v8::Exception::TypeError(msg.IsEmpty() ? v8::String::Empty(isolate) : msg.ToLocalChecked()));
  }

  void defineHiddenField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(false);

    auto nameMaybe = v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized);
    if (!nameMaybe.IsEmpty()) {
      ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), nameMaybe.ToLocalChecked(), propertyDescriptor));
    }
  }

  void defineReadonlyField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(true);

    auto nameMaybe = v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized);
    if (!nameMaybe.IsEmpty()) {
      ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), nameMaybe.ToLocalChecked(), propertyDescriptor));
    }
  }

  void defineHiddenFunction(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::FunctionCallback callback) {
    v8::HandleScope scope(isolate);
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback);
    t->SetClassName(v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked());
    defineHiddenField(isolate, target, name, t->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
  }

  /////////////// AsyncWorker ///////////////

  AsyncWorker::AsyncWorker(v8::Isolate * isolate) : isolate(isolate), _error(nullptr), _completed(false) {
    _task.data = this;
  }

  AsyncWorker::~AsyncWorker() {
    _callback.Reset();
    _resolver.Reset();
  }

  bool AsyncWorker::setCallback(v8::Local<v8::Value> callback) {
    if (callback.IsEmpty() || !callback->IsFunction()) {
      return false;
    }
    _callback.Reset(isolate, v8::Local<v8::Function>::Cast(callback));
    return true;
  }

  bool AsyncWorker::_start() {
    if (uv_queue_work(uv_default_loop(), &_task, AsyncWorker::_work, AsyncWorker::_done) != 0) {
      setError("Error starting async thread");
      return false;
    }

    return true;
  }

  v8::Local<v8::Value> AsyncWorker::run(AsyncWorker * worker) {
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

      worker->_resolver.Reset(isolate, resolver);
    }

    if (!worker->_start()) {
      _resolveOrReject(worker);
    }

    return returnValue;
  }

  v8::Local<v8::Value> AsyncWorker::done() {
    return v8::Local<v8::Value>();
  }

  void AsyncWorker::_work(uv_work_t * request) {
    AsyncWorker * worker = static_cast<AsyncWorker *>(request->data);
    if (!worker->hasError()) {
      worker->work();
    }
  }

  void AsyncWorker::_done(uv_work_t * request, int status) {
    AsyncWorker * worker = static_cast<AsyncWorker *>(request->data);
    if (status != 0) {
      worker->setError("Error executing async thread");
    }
    _complete(worker);
  }

  v8::Local<v8::Value> AsyncWorker::_invokeDone() {
    v8::EscapableHandleScope scope(isolate);

    bool isError = false;

    v8::Local<v8::Value> result;
    if (_error == nullptr) {
      v8::TryCatch tryCatch(isolate);

      result = done();

      if (tryCatch.HasCaught()) {
        isError = true;
        result = tryCatch.Exception();
        if (result.IsEmpty() || result->IsNull() || result->IsUndefined()) {
          setError("Exception in async operation");
        } else if (!result->IsObject()) {
          v8::MaybeLocal<v8::String> message = result->ToString(isolate->GetCurrentContext());
          if (message.IsEmpty()) {
            message = v8::String::NewFromUtf8(isolate, "Operation failed", v8::NewStringType::kInternalized);
          }
          result = v8::Exception::Error(message.IsEmpty() ? v8::String::Empty(isolate) : message.ToLocalChecked());
        }
      }
    }

    if (_error != nullptr && result.IsEmpty()) {
      isError = true;
      v8::MaybeLocal<v8::String> message = v8::String::NewFromUtf8(isolate, _error, v8::NewStringType::kNormal);
      result = v8::Exception::Error(message.IsEmpty() ? v8::String::Empty(isolate) : message.ToLocalChecked());
    }

    if (isError && _error == nullptr) {
      _error = "Async generated an exception";
    }

    return scope.Escape(result);
  }

  void AsyncWorker::_resolveOrReject(AsyncWorker * worker) {
    if (worker->_completed) {
      return;
    }

    worker->_completed = true;

    v8::Isolate * isolate = worker->isolate;
    v8::HandleScope scope(isolate);

    v8::Local<v8::Value> result = worker->_invokeDone();

    bool hasError = worker->hasError();
    auto context = isolate->GetCurrentContext();
    if (worker->_resolver.IsEmpty()) {
      v8::Local<v8::Function> callback = worker->_callback.Get(isolate);
      delete worker;
      if (hasError) {
        v8::Local<v8::Value> argv[] = {result, v8::Undefined(isolate)};
        callback->Call(context, context->Global(), 2, argv).ToLocalChecked();
      } else {
        v8::Local<v8::Value> argv[] = {v8::Null(isolate), result};
        callback->Call(context, context->Global(), 2, argv).ToLocalChecked();
      }
    } else {
      v8::Local<v8::Promise::Resolver> resolver = worker->_resolver.Get(isolate);
      delete worker;
      if (hasError) {
        v8utils::ignoreMaybeResult(resolver->Reject(context, result));
      } else {
        v8utils::ignoreMaybeResult(resolver->Resolve(context, result));
      }
    }
  }

  void AsyncWorker::_complete(AsyncWorker * worker) {
    v8::Isolate * isolate = worker->isolate;
    _resolveOrReject(worker);
#if NODE_MAJOR_VERSION > 13
    isolate->PerformMicrotaskCheckpoint();
#else
    isolate->RunMicrotasks();
#endif
  }

  /////////////// ParallelAsyncWorker ///////////////

  ParallelAsyncWorker::ParallelAsyncWorker(v8::Isolate * isolate) :
      AsyncWorker(isolate), loopCount(0), concurrency(0), _tasks(nullptr), _pendingTasks(0), _currentIndex(0) {
  }

  ParallelAsyncWorker::~ParallelAsyncWorker() {
    if (_tasks != nullptr) {
      delete[] _tasks;
    }
  }

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

    uint32_t tasksCount = std::min(concurrency, loopCount);

    if (tasksCount <= 1) {
      return AsyncWorker::_start();
    }

    uv_work_t * tasks = new uv_work_t[tasksCount]();
    if (tasks == nullptr) {
      this->setError("Failed to allocate memory");
      return false;
    }

    _tasks = tasks;

    for (uint32_t taskIndex = 0; taskIndex != tasksCount; ++taskIndex) {
      tasks[taskIndex].data = this;
    }

    for (uint32_t taskIndex = 0; taskIndex != tasksCount; ++taskIndex) {
      if (uv_queue_work(uv_default_loop(), &tasks[taskIndex], ParallelAsyncWorker::_parallelWork, ParallelAsyncWorker::_parallelDone) != 0) {
        setError("Error starting async parallel task");
        break;
      } else {
        ++_pendingTasks;
      }
    }

    return _pendingTasks > 0;
  }

  void ParallelAsyncWorker::_parallelWork(uv_work_t * request) {
    ParallelAsyncWorker * worker = static_cast<ParallelAsyncWorker *>(request->data);

    uint32_t loopCount = worker->loopCount;
    while (!worker->hasError() && !worker->_completed) {
      const uint32_t prevIndex = worker->_currentIndex;
      const uint32_t index = atomicIncrement32(&worker->_currentIndex) - 1;
      if (index >= loopCount || index < prevIndex) {
        break;
      }
      worker->parallelWork(index);
    }
  }

  void ParallelAsyncWorker::_parallelDone(uv_work_t * request, int status) {
    ParallelAsyncWorker * worker = static_cast<ParallelAsyncWorker *>(request->data);

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

#include "v8utils.h"

#include <stdlib.h>

#ifdef _MSC_VER
#  define atomicIncrement32(ptr) InterlockedIncrement(ptr)
#  define atomicDecrement32(ptr) InterlockedDecrement(ptr)
#else
#  define atomicIncrement32(ptr) __sync_add_and_fetch(ptr, 1)
#  define atomicDecrement32(ptr) __sync_sub_and_fetch(ptr, 1)
#endif

#if defined(__APPLE__)
#  include <malloc/malloc.h>
#else
#  include <malloc.h>
#endif

/** portable version of posix_memalign */
void * bare_aligned_malloc(size_t alignment, size_t size) {
  void * p;
#ifdef _MSC_VER
  p = _aligned_malloc(size, alignment);
#elif defined(__MINGW32__) || defined(__MINGW64__)
  p = __mingw_aligned_malloc(size, alignment);
#else
  // somehow, if this is used before including "x86intrin.h", it creates an
  // implicit defined warning.
  if (posix_memalign(&p, alignment, size) != 0) return NULL;
#endif
  return p;
}

/** portable version of free fo aligned allocs */
void bare_aligned_free(void * memblock) {
  if (memblock != nullptr) {
#ifdef _MSC_VER
    _aligned_free(memblock);
#elif defined(__MINGW32__) || defined(__MINGW64__)
    __mingw_aligned_free(memblock);
#else
    free(memblock);
#endif
  }
}

/** portable version of malloc_size */
inline static size_t bare_malloc_size(void * ptr) {
#if defined(__APPLE__)
  return malloc_size(ptr);
#elif defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
  return _msize(ptr);
#else
  return malloc_usable_size(ptr);
#endif
}

/** portable version of malloc_size for memory allocated with bare_aligned_malloc */
inline static size_t bare_aligned_malloc_size(void * ptr) {
#if defined(__APPLE__)
  return malloc_size(ptr);
#elif defined(_WIN32)
  return _aligned_msize(ptr, 32, 0);
#else
  return malloc_usable_size(ptr);
#endif
}

std::atomic<int64_t> gcaware_totalMemCounter{0};

int64_t gcaware_totalMem() { return gcaware_totalMemCounter.load(std::memory_order_relaxed); }

#include <execinfo.h>
#include <iostream>

static thread_local v8::Isolate * thread_local_isolate = nullptr;

void gcaware_adjustAllocatedMemory(int64_t size) {
  if (size != 0) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    if (isolate == nullptr) {
      isolate = thread_local_isolate;
    }
    if (isolate != nullptr) {
      isolate->AdjustAmountOfExternalAllocatedMemory(size);
    }
    int64_t old = gcaware_totalMemCounter;
    gcaware_totalMemCounter += size;

    if (old >= 0 && gcaware_totalMemCounter < 0) {
      std::cout << std::endl;
      void * callstack[128];
      int i, frames = backtrace(callstack, 128);
      char ** strs = backtrace_symbols(callstack, frames);
      for (i = 0; i < frames; ++i) {
        printf("%s\n", strs[i]);
      }
      free(strs);
      std::cout << "old: " << old << " gcaware_totalMemCounter: " << gcaware_totalMemCounter << std::endl << std::endl;
    }
  }
}

void * gcaware_malloc(size_t size) {
  void * memory = malloc(size);
  if (memory != nullptr) {
    gcaware_adjustAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

void * gcaware_realloc(void * memory, size_t size) {
  size_t oldSize = memory != nullptr ? bare_malloc_size(memory) : 0;
  memory = realloc(memory, size);
  if (memory != nullptr) {
    gcaware_adjustAllocatedMemory(-oldSize);
    gcaware_adjustAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

void * gcaware_calloc(size_t count, size_t size) {
  void * memory = calloc(count, size);
  if (memory != nullptr) {
    gcaware_adjustAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

void gcaware_free(void * memory) {
  if (memory != nullptr) {
    gcaware_adjustAllocatedMemory(-bare_malloc_size(memory));
  }
  free(memory);
}

void * gcaware_aligned_malloc(size_t alignment, size_t size) {
  void * memory = bare_aligned_malloc(alignment, size);
  if (memory != nullptr) {
    gcaware_adjustAllocatedMemory(bare_aligned_malloc_size(memory));
  }
  return memory;
}

void gcaware_aligned_free(void * memory) {
  if (memory != nullptr) {
    gcaware_adjustAllocatedMemory(-bare_aligned_malloc_size(memory));
  }
  bare_aligned_free(memory);
}

/////////////// JSTypes ///////////////

v8::Eternal<v8::Object> JSTypes::Uint32Array;
v8::Eternal<v8::Function> JSTypes::Uint32Array_from;

void JSTypes::initJSTypes(v8::Isolate * isolate, const v8::Local<v8::Object> & global) {
  v8::HandleScope scope(isolate);
  auto context = isolate->GetCurrentContext();

  auto uint32Array = global->Get(context, NEW_LITERAL_V8_STRING(isolate, "Uint32Array", v8::NewStringType::kInternalized))
                       .ToLocalChecked()
                       ->ToObject(context)
                       .ToLocalChecked();

  JSTypes::Uint32Array.Set(isolate, uint32Array);
  JSTypes::Uint32Array_from.Set(
    isolate,
    v8::Local<v8::Function>::Cast(
      uint32Array->Get(context, NEW_LITERAL_V8_STRING(isolate, "from", v8::NewStringType::kInternalized)).ToLocalChecked()));
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

  /////////////// AsyncWorker ///////////////

  AsyncWorker::AsyncWorker(v8::Isolate * isolate) : isolate(isolate), _error(nullptr), _completed(false) {
    _task.data = this;
  }

  AsyncWorker::~AsyncWorker() {}

  bool AsyncWorker::setCallback(v8::Local<v8::Value> callback) {
    if (callback.IsEmpty() || !callback->IsFunction()) return false;
    _callback.Reset(isolate, v8::Local<v8::Function>::Cast(callback));
    return true;
  }

  bool AsyncWorker::_start() {
    if (this->hasError()) {
      return false;
    }
    this->before();
    if (this->hasError()) {
      return false;
    }
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

  void AsyncWorker::before() {}

  v8::Local<v8::Value> AsyncWorker::done() { return {}; }

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

  v8::Local<v8::Value> AsyncWorker::_invokeDone() {
    v8::EscapableHandleScope scope(isolate);

    bool isError = false;

    v8::Local<v8::Value> result;
    if (_error == nullptr) {
      v8::TryCatch tryCatch(isolate);

      result = this->done();

      if (tryCatch.HasCaught()) {
        isError = true;
        result = tryCatch.Exception();
        if (result.IsEmpty() || result->IsNull() || result->IsUndefined()) {
          setError("Exception in async operation");
        } else if (!result->IsObject()) {
          v8::MaybeLocal<v8::String> message = result->ToString(isolate->GetCurrentContext());
          if (message.IsEmpty()) {
            message = NEW_LITERAL_V8_STRING(isolate, "Operation failed", v8::NewStringType::kInternalized);
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

    this->finally();

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
      std::atomic_thread_fence(std::memory_order_seq_cst);
      delete worker;
      if (hasError) {
        v8::Local<v8::Value> argv[] = {result, v8::Undefined(isolate)};
        v8utils::ignoreMaybeResult(callback->Call(context, context->Global(), 2, argv));
      } else {
        v8::Local<v8::Value> argv[] = {v8::Null(isolate), result};
        v8utils::ignoreMaybeResult(callback->Call(context, context->Global(), 2, argv));
      }
    } else {
      v8::Local<v8::Promise::Resolver> resolver = worker->_resolver.Get(isolate);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      delete worker;
      if (hasError) {
        v8utils::ignoreMaybeResult(resolver->Reject(context, result));
      } else {
        v8utils::ignoreMaybeResult(resolver->Resolve(context, result));
      }
    }
  }

  void AsyncWorker::_complete(AsyncWorker * worker) {
    std::atomic_thread_fence(std::memory_order_seq_cst);
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

#ifndef __V8UTILS__H__
#define __V8UTILS__H__

#include <node.h>
#include <uv.h>
#include <algorithm>
#include <cmath>
#include <string>

class NonCopyable {
 protected:
  constexpr NonCopyable() throw() = default;
  ~NonCopyable() throw() = default;

 private:
  NonCopyable(const NonCopyable &) throw() = delete;
  void operator=(const NonCopyable &) throw() = delete;
};

class JSTypes {
 public:
  static v8::Eternal<v8::Object> Array;
  static v8::Eternal<v8::Function> Array_from;

  static v8::Eternal<v8::Object> Buffer;
  static v8::Eternal<v8::Function> Buffer_allocUnsafe;

  static v8::Eternal<v8::Object> Uint32Array;
  static v8::Eternal<v8::Function> Uint32Array_ctor;
  static v8::Eternal<v8::Function> Uint32Array_from;

  static v8::Eternal<v8::Object> Set;
  static v8::Eternal<v8::Function> Set_ctor;

  static void initJSTypes(v8::Isolate * isolate, const v8::Local<v8::Object> & global);

  static v8::Local<v8::Value> bufferAllocUnsafe(v8::Isolate * isolate, size_t size);
};

namespace v8utils {

  uint32_t getCpusCount();

  template <typename T>
  inline void ignoreMaybeResult(v8::Maybe<T>) {
  }

  void throwError(v8::Isolate * isolate, const char * message);

  void throwTypeError(v8::Isolate * isolate, const char * message);

  inline void throwTypeError(v8::Isolate * isolate, const std::string & message) {
    throwTypeError(isolate, message.c_str());
  }

  void defineHiddenField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value);
  void defineReadonlyField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value);
  void defineHiddenFunction(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::FunctionCallback callback);

  template <typename T>
  struct TypedArrayContent {
    size_t length;
    T * data;
    v8::Local<v8::ArrayBuffer> arrayBuffer;
#if NODE_MAJOR_VERSION > 13
    std::shared_ptr<v8::BackingStore> backingStore;
#endif

    inline TypedArrayContent() : length(0), data(nullptr) {
    }

    inline TypedArrayContent(TypedArrayContent<T> & copy) : length(copy.length), data(copy.data) {
    }

    inline TypedArrayContent(v8::Local<v8::Value> from) {
      set(from);
    }

    inline void reset() {
      this->length = 0;
      this->data = nullptr;
#if NODE_MAJOR_VERSION > 13
      this->backingStore = nullptr;
#endif
      this->arrayBuffer.Clear();
    }

    inline bool set(v8::Local<v8::Value> from) {
      if (!from.IsEmpty() && from->IsArrayBufferView()) {
        auto array = v8::Local<v8::ArrayBufferView>::Cast(from);
        this->length = array->ByteLength() / sizeof(T);
        this->arrayBuffer = array->Buffer();
#if NODE_MAJOR_VERSION > 13
        this->backingStore = this->arrayBuffer->GetBackingStore();
        this->data = (T *)((uint8_t *)(this->backingStore->Data()) + array->ByteOffset());
#else
        this->data = (T *)((uint8_t *)(this->arrayBuffer->GetContents().Data()) + array->ByteOffset());
#endif
        return true;
      }

      this->length = 0;
      this->data = nullptr;
      return false;
    }
  };

  class ObjectWrap {
   public:
    template <class T>
    inline static T * Unwrap(v8::Local<v8::Object> object) {
      return (T *)(object->GetAlignedPointerFromInternalField(0));
    }

    template <class T>
    static T * TryUnwrap(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
      v8::Local<v8::Object> obj;
      if (!value->ToObject(isolate->GetCurrentContext()).ToLocal(&obj)) {
        return nullptr;
      }
      return (T *)(obj->GetAlignedPointerFromInternalField(0));
    }

    template <class T>
    static T * TryUnwrap(const v8::Local<v8::Value> & value, const v8::Local<v8::FunctionTemplate> & ctorTemplate, v8::Isolate * isolate) {
      return !ctorTemplate.IsEmpty() && ctorTemplate->HasInstance(value) ? ObjectWrap::TryUnwrap<T>(value, isolate) : nullptr;
    }

    template <class T>
    static T * TryUnwrap(const v8::Local<v8::Value> & value, const v8::Persistent<v8::FunctionTemplate> & ctorTemplate, v8::Isolate * isolate) {
      return ObjectWrap::TryUnwrap<T>(value, ctorTemplate.Get(isolate), isolate);
    }

    template <class T>
    static T * TryUnwrap(const v8::Local<v8::Value> & value, const v8::Eternal<v8::FunctionTemplate> & ctorTemplate, v8::Isolate * isolate) {
      return ObjectWrap::TryUnwrap<T>(value, ctorTemplate.Get(isolate), isolate);
    }

    template <class T>
    static T * TryUnwrap(const v8::FunctionCallbackInfo<v8::Value> & info, int argumentIndex, const v8::Local<v8::FunctionTemplate> & ctorTemplate) {
      return info.Length() <= argumentIndex ? nullptr : ObjectWrap::TryUnwrap<T>(info[argumentIndex], ctorTemplate, info.GetIsolate());
    }

    template <class T>
    static T * TryUnwrap(const v8::FunctionCallbackInfo<v8::Value> & info, int argumentIndex, const v8::Persistent<v8::FunctionTemplate> & ctorTemplate) {
      return info.Length() <= argumentIndex ? nullptr : ObjectWrap::TryUnwrap<T>(info[argumentIndex], ctorTemplate, info.GetIsolate());
    }

    template <class T>
    static T * TryUnwrap(const v8::FunctionCallbackInfo<v8::Value> & info, int argumentIndex, const v8::Eternal<v8::FunctionTemplate> & ctorTemplate) {
      return info.Length() <= argumentIndex ? nullptr : ObjectWrap::TryUnwrap<T>(info[argumentIndex], ctorTemplate, info.GetIsolate());
    }
  };

  typedef const char * const_char_ptr_t;

  class AsyncWorker {
   public:
    v8::Isolate * const isolate;

    AsyncWorker(v8::Isolate * isolate);

    virtual ~AsyncWorker();

    bool setCallback(v8::Local<v8::Value> callback);

    inline bool hasError() const {
      return this->_error != nullptr;
    }

    inline void setError(const_char_ptr_t error) {
      if (error != nullptr && this->_error == nullptr) {
        this->_error = error;
      }
    }

    inline void clearError() {
      this->_error = nullptr;
    }

    static v8::Local<v8::Value> run(AsyncWorker * worker);

   protected:
    // Called in a thread to execute the workload
    virtual void work() = 0;

    // Called after the thread completes without errors.
    virtual v8::Local<v8::Value> done();

    inline v8::Local<v8::Value> empty() const {
      return v8::Local<v8::Value>();
    }

   private:
    uv_work_t _task;
    volatile const_char_ptr_t _error;
    volatile bool _completed;
    v8::Persistent<v8::Function> _callback;
    v8::Persistent<v8::Promise::Resolver> _resolver;

    v8::Local<v8::Value> _invokeDone();
    virtual bool _start();
    static void _complete(AsyncWorker * worker);
    static void _resolveOrReject(AsyncWorker * worker);
    static void _work(uv_work_t * request);
    static void _done(uv_work_t * request, int status);

    friend class ParallelAsyncWorker;
  };

  class ParallelAsyncWorker : public AsyncWorker {
   public:
    uint32_t loopCount;
    uint32_t concurrency;

    ParallelAsyncWorker(v8::Isolate * isolate);
    virtual ~ParallelAsyncWorker();

   protected:
    virtual void work();

    virtual void parallelWork(uint32_t index) = 0;

   private:
    uv_work_t * _tasks;
    volatile int32_t _pendingTasks;
    volatile uint32_t _currentIndex;

    virtual bool _start();

    static void _parallelWork(uv_work_t * request);
    static void _parallelDone(uv_work_t * request, int status);
  };

}  // namespace v8utils

#endif
#ifndef ROARING_NODE_V8UTILS_
#define ROARING_NODE_V8UTILS_

#include "includes.h"
#include "addon-data.h"
#include "addon-strings.h"
#include "memory.h"

namespace v8utils {

  template <typename T>
  inline void ignoreMaybeResult(v8::Maybe<T>) {}

  template <typename T>
  inline void ignoreMaybeResult(v8::MaybeLocal<T>) {}

  template <int N>
  inline void throwError(v8::Isolate * isolate, const char (&message)[N]) {
    isolate->ThrowException(v8::Exception::Error(NEW_LITERAL_V8_STRING(isolate, message, v8::NewStringType::kInternalized)));
  }

  template <int N>
  inline void throwTypeError(v8::Isolate * isolate, const char (&message)[N]) {
    isolate->ThrowException(
      v8::Exception::TypeError(NEW_LITERAL_V8_STRING(isolate, message, v8::NewStringType::kInternalized)));
  }

  void throwError(v8::Isolate * isolate, const char * message);
  void throwTypeError(v8::Isolate * isolate, const char * message);
  void throwTypeError(v8::Isolate * isolate, const char * context, const char * message);

  template <int N>
  void defineHiddenField(
    v8::Isolate * isolate, v8::Local<v8::Object> target, const char (&literal)[N], v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(false);

    auto name = NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized);
    ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), name, propertyDescriptor));
  }

  template <int N>
  void defineReadonlyField(
    v8::Isolate * isolate, v8::Local<v8::Object> target, const char (&literal)[N], v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(true);

    auto name = NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized);
    ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), name, propertyDescriptor));
  }

  template <int N>
  void defineHiddenFunction(
    v8::Isolate * isolate, v8::Local<v8::Object> target, const char (&literal)[N], v8::FunctionCallback callback) {
    v8::HandleScope scope(isolate);
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback);

    auto name = NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized);
    t->SetClassName(name);

    auto fn = t->GetFunction(isolate->GetCurrentContext());
    v8::Local<v8::Function> fnLocal;
    if (fn.ToLocal(&fnLocal)) {
      v8::PropertyDescriptor propertyDescriptor(fnLocal, false);
      propertyDescriptor.set_configurable(false);
      propertyDescriptor.set_enumerable(false);
      ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), name, propertyDescriptor));
    }
  }

  bool v8ValueToBufferWithLimit(
    v8::Isolate * isolate, v8::MaybeLocal<v8::Value> value, size_t length, v8::Local<v8::Value> & result);

  bool v8ValueToUint32ArrayWithLimit(
    v8::Isolate * isolate, v8::MaybeLocal<v8::Value> value, size_t length, v8::Local<v8::Value> & result);

  template <typename T>
  class TypedArrayContent final {
   public:
    size_t length;
    T * data;
    v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bufferPersistent;
#if NODE_MAJOR_VERSION > 13
    std::shared_ptr<v8::BackingStore> backingStore;
#endif

    inline TypedArrayContent() : length(0), data(nullptr) {}

    template <typename Q>
    inline explicit TypedArrayContent(v8::Isolate * isolate, v8::Local<Q> from) {
      set(isolate, from);
    }

    template <typename Q>
    inline explicit TypedArrayContent(v8::Isolate * isolate, v8::MaybeLocal<Q> from) {
      set(isolate, from);
    }

    inline void reset() {
      this->length = 0;
      this->data = nullptr;
#if NODE_MAJOR_VERSION > 13
      this->backingStore = nullptr;
#endif
      this->bufferPersistent.Reset();
    }

    template <typename Q>
    bool set(v8::Isolate * isolate, const v8::MaybeLocal<Q> & from) {
      v8::Local<Q> local;
      if (from.ToLocal(&local)) {
        return this->set(isolate, local);
      }
      this->reset();
      return false;
    }

    template <typename Q>
    bool set(v8::Isolate * isolate, const v8::Local<Q> & from) {
      if (!from.IsEmpty()) {
        if (from->IsArrayBufferView()) {
          bufferPersistent.Reset(isolate, from);
          v8::Local<v8::ArrayBufferView> array = v8::Local<v8::ArrayBufferView>::Cast(from);
          this->length = array->ByteLength() / sizeof(T);
          auto arrayBuffer = array->Buffer();
#if NODE_MAJOR_VERSION > 13
          this->backingStore = arrayBuffer->GetBackingStore();
          this->data = (T *)((uint8_t *)(this->backingStore->Data()) + array->ByteOffset());
#else
          this->data = (T *)((uint8_t *)(arrayBuffer->GetContents().Data()) + array->ByteOffset());
#endif
          return true;
        }

        if (from->IsArrayBuffer()) {
          bufferPersistent.Reset(isolate, from);
          v8::Local<v8::ArrayBuffer> arrayBuffer = v8::Local<v8::ArrayBuffer>::Cast(from);
          this->length = arrayBuffer->ByteLength() / sizeof(T);
#if NODE_MAJOR_VERSION > 13
          this->backingStore = arrayBuffer->GetBackingStore();
          this->data = (T *)((uint8_t *)(this->backingStore->Data()));
#else
          this->data = (T *)((uint8_t *)(arrayBuffer->GetContents().Data()));
#endif
          return true;
        }
      }

      this->reset();
      return false;
    }
  };

  class AsyncWorker {
   public:
    v8::Isolate * const isolate;

    explicit AsyncWorker(v8::Isolate * isolate);

    virtual ~AsyncWorker();

    bool setCallback(v8::Local<v8::Value> callback);

    inline bool hasStarted() const { return this->_started; }

    inline bool hasError() const { return this->_error != nullptr; }

    inline void setError(const_char_ptr_t error) {
      if (error != nullptr && this->_error == nullptr) {
        this->_error = error;
      }
    }

    inline void clearError() { this->_error = nullptr; }

    static v8::Local<v8::Value> run(AsyncWorker * worker);

   protected:
    // Called before the thread starts, in the main thread.
    virtual void before();

    // Called in a thread to execute the workload
    virtual void work() = 0;

    // Called after the thread completes without errors, in the main thread.
    virtual void done(v8::Local<v8::Value> & result);

    // Called after the thread completes, with or without errors, in the main thread.
    virtual void finally();

   private:
    uv_work_t _task{};
    volatile const_char_ptr_t _error;
    bool _started;
    volatile bool _completed;
    v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> _callback;
    v8::Persistent<v8::Promise::Resolver, v8::CopyablePersistentTraits<v8::Promise::Resolver>> _resolver;

    virtual bool _start();
    static void _complete(AsyncWorker * worker);
    static void _resolveOrReject(AsyncWorker * worker, v8::Local<v8::Value> & error);
    static void _work(uv_work_t * request);
    static void _done(uv_work_t * request, int status);
    v8::Local<v8::Value> _makeError(v8::Local<v8::Value> error);

    friend class ParallelAsyncWorker;
  };

  class ParallelAsyncWorker : public AsyncWorker {
   public:
    uint32_t loopCount;
    uint32_t concurrency;

    explicit ParallelAsyncWorker(v8::Isolate * isolate);
    virtual ~ParallelAsyncWorker();

   protected:
    void work() override;

    virtual void parallelWork(uint32_t index) = 0;

   private:
    uv_work_t * _tasks;
    volatile int32_t _pendingTasks;
    volatile uint32_t _currentIndex;

    bool _start() override;

    static void _parallelWork(uv_work_t * request);
    static void _parallelDone(uv_work_t * request, int status);
  };

}  // namespace v8utils

#endif  // ROARING_NODE_V8UTILS_

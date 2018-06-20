#ifndef __V8UTILS__H__
#define __V8UTILS__H__

#include <node.h>

class TypedArrays {
 public:
  static v8::Persistent<v8::Object> Array;
  static v8::Persistent<v8::Function> Array_from;

  static v8::Persistent<v8::Object> Buffer;
  static v8::Persistent<v8::Function> Buffer_allocUnsafe;

  static v8::Persistent<v8::Object> Uint32Array;
  static v8::Persistent<v8::Function> Uint32Array_ctor;
  static v8::Persistent<v8::Function> Uint32Array_from;

  static v8::Persistent<v8::Object> Set;
  static v8::Persistent<v8::Function> Set_ctor;

  static void initTypedArrays(v8::Isolate * isolate, const v8::Local<v8::Object> & global);

  static v8::Local<v8::Value> bufferAllocUnsafe(v8::Isolate * isolate, size_t size);
};

namespace v8utils {

  void throwError(const char * message);
  void throwTypeError(const char * message);

  void defineHiddenField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value);
  void defineReadonlyField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value);
  void defineHiddenFunction(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::FunctionCallback callback);

  template <typename T>
  struct TypedArrayContent {
    size_t length;
    T * data;

    inline TypedArrayContent(v8::Isolate * isolate, v8::Local<v8::Value> from) : length(0), data(NULL) {
      v8::HandleScope scope(isolate);
      if (from->IsArrayBufferView()) {
        v8::Local<v8::ArrayBufferView> array = v8::Local<v8::ArrayBufferView>::Cast(from);
        this->length = array->ByteLength() / sizeof(T);
        this->data = (T *)((char *)(array->Buffer()->GetContents().Data()) + array->ByteOffset());
      }
    }
  };

  class ObjectWrap {
   public:
    v8::Persistent<v8::Object> persistent;

    inline void Wrap(v8::Isolate * isolate, v8::Local<v8::Object> object) {
      object->SetAlignedPointerInInternalField(0, this);
      persistent.Reset(isolate, object);
      persistent.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);
      persistent.MarkIndependent();
    }

    template <class T>
    inline static T * Unwrap(v8::Local<v8::Object> object) {
      return (T *)(object->GetAlignedPointerFromInternalField(0));
    }

   protected:
    virtual ~ObjectWrap();

    static void WeakCallback(v8::WeakCallbackInfo<ObjectWrap> const & info);
  };

}  // namespace v8utils

#endif
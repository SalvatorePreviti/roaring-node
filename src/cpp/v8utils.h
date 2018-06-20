#ifndef __V8UTILS__H__
#define __V8UTILS__H__

#include <nan.h>

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

  void defineHiddenField(v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value);
  void defineReadonlyField(v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value);
  void defineHiddenFunction(v8::Local<v8::Object> target, const char * name, v8::FunctionCallback callback);

  template <typename T>
  struct TypedArrayContent {
    size_t length;
    T * data;

    inline explicit TypedArrayContent(v8::Isolate * isolate, v8::Local<v8::Value> from) : length(0), data(NULL) {
      v8::HandleScope scope(isolate);

      size_t length = 0;
      void * data = NULL;

      if (from->IsArrayBufferView()) {
        v8::Local<v8::ArrayBufferView> array = v8::Local<v8::ArrayBufferView>::Cast(from);

        const size_t byte_length = array->ByteLength();
        const ptrdiff_t byte_offset = array->ByteOffset();
        v8::Local<v8::ArrayBuffer> buffer = array->Buffer();

        length = byte_length / sizeof(T);
        data = static_cast<char *>(buffer->GetContents().Data()) + byte_offset;
      }

      this->length = length;
      this->data = static_cast<T *>(data);
    }
  };

}  // namespace v8utils

#endif
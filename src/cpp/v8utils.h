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

}  // namespace v8utils

#endif
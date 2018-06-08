#ifndef __TYPEDARRAYS__H__
#define __TYPEDARRAYS__H__

#include <nan.h>

class TypedArrays {
 public:
  static Nan::Persistent<v8::Object> Array;
  static Nan::Persistent<v8::Function> Array_from;

  static Nan::Persistent<v8::Object> Buffer;
  static Nan::Persistent<v8::Function> Buffer_allocUnsafe;

  static Nan::Persistent<v8::Object> Uint32Array;
  static Nan::Persistent<v8::Function> Uint32Array_ctor;
  static Nan::Persistent<v8::Function> Uint32Array_from;

  static void initTypedArrays(const v8::Local<v8::Object> & global);

  static v8::Local<v8::Value> bufferAllocUnsafe(v8::Isolate * isolate, size_t size);
};

#endif
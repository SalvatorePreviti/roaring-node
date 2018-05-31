#ifndef __TYPEDARRAYS__H__
#define __TYPEDARRAYS__H__

#include <nan.h>

class TypedArrays {
 public:
  static Nan::Persistent<v8::Object> Uint32Array;
  static Nan::Persistent<v8::Function> Uint32Array_ctor;
  static Nan::Persistent<v8::Function> Uint32Array_from;

  static Nan::Persistent<v8::Object> Uint8Array;
  static Nan::Persistent<v8::Function> Uint8Array_ctor;

  static void initTypedArrays(const v8::Local<v8::Object> & global);
};

#endif
#include "TypedArrays.h"

Nan::Persistent<v8::Object> TypedArrays::Uint32Array;
Nan::Persistent<v8::Function> TypedArrays::Uint32Array_ctor;
Nan::Persistent<v8::Function> TypedArrays::Uint32Array_from;

Nan::Persistent<v8::Object> TypedArrays::Buffer;
Nan::Persistent<v8::Function> TypedArrays::Buffer_allocUnsafe;

Nan::Persistent<v8::Object> TypedArrays::Array;
Nan::Persistent<v8::Function> TypedArrays::Array_from;

void TypedArrays::initTypedArrays(const v8::Local<v8::Object> & global) {
  auto uint32Array = Nan::Get(global, Nan::New("Uint32Array").ToLocalChecked()).ToLocalChecked()->ToObject();
  TypedArrays::Uint32Array.Reset(uint32Array);
  TypedArrays::Uint32Array_ctor.Reset(v8::Local<v8::Function>::Cast(uint32Array));
  TypedArrays::Uint32Array_from.Reset(v8::Local<v8::Function>::Cast(Nan::Get(uint32Array, Nan::New("from").ToLocalChecked()).ToLocalChecked()));

  auto buffer = Nan::Get(global, Nan::New("Buffer").ToLocalChecked()).ToLocalChecked()->ToObject();
  TypedArrays::Buffer.Reset(buffer);
  TypedArrays::Buffer_allocUnsafe.Reset(v8::Local<v8::Function>::Cast(Nan::Get(buffer, Nan::New("allocUnsafe").ToLocalChecked()).ToLocalChecked()));

  auto array = Nan::Get(global, Nan::New("Array").ToLocalChecked()).ToLocalChecked()->ToObject();
  TypedArrays::Array.Reset(array);
  TypedArrays::Array_from.Reset(v8::Local<v8::Function>::Cast(Nan::Get(array, Nan::New("from").ToLocalChecked()).ToLocalChecked()));
}

v8::Local<v8::Value> TypedArrays::bufferAllocUnsafe(v8::Isolate * isolate, size_t size) {
  v8::Local<v8::Value> argv[] = {{Nan::New((double)size)}};
  return TypedArrays::Buffer_allocUnsafe.Get(isolate)->Call(TypedArrays::Uint32Array.Get(isolate), 1, argv);
}

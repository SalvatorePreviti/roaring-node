#include "TypedArrays.h"

Nan::Persistent<v8::Object> TypedArrays::Uint32Array;
Nan::Persistent<v8::Function> TypedArrays::Uint32Array_ctor;
Nan::Persistent<v8::Function> TypedArrays::Uint32Array_from;

Nan::Persistent<v8::Object> TypedArrays::Uint8Array;
Nan::Persistent<v8::Function> TypedArrays::Uint8Array_ctor;

void TypedArrays::initTypedArrays(const v8::Local<v8::Object> & global) {
  auto uint32Array = Nan::Get(global, Nan::New("Uint32Array").ToLocalChecked()).ToLocalChecked()->ToObject();
  TypedArrays::Uint32Array.Reset(uint32Array);
  TypedArrays::Uint32Array_ctor.Reset(v8::Local<v8::Function>::Cast(uint32Array));
  TypedArrays::Uint32Array_from.Reset(v8::Local<v8::Function>::Cast(Nan::Get(uint32Array, Nan::New("from").ToLocalChecked()).ToLocalChecked()));

  auto uint8Array = Nan::Get(global, Nan::New("Uint8Array").ToLocalChecked()).ToLocalChecked()->ToObject();
  TypedArrays::Uint8Array.Reset(uint8Array);
  TypedArrays::Uint8Array_ctor.Reset(v8::Local<v8::Function>::Cast(uint8Array));
}

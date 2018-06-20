#include "v8utils.h"

Nan::Persistent<v8::Object> TypedArrays::Uint32Array;
Nan::Persistent<v8::Function> TypedArrays::Uint32Array_ctor;
Nan::Persistent<v8::Function> TypedArrays::Uint32Array_from;

Nan::Persistent<v8::Object> TypedArrays::Buffer;
Nan::Persistent<v8::Function> TypedArrays::Buffer_allocUnsafe;

Nan::Persistent<v8::Object> TypedArrays::Array;
Nan::Persistent<v8::Function> TypedArrays::Array_from;

Nan::Persistent<v8::Object> TypedArrays::Set;
Nan::Persistent<v8::Function> TypedArrays::Set_ctor;

void TypedArrays::initTypedArrays(v8::Isolate * isolate, const v8::Local<v8::Object> & global) {
  auto uint32Array = Nan::Get(global, v8::String::NewFromUtf8(isolate, "Uint32Array")).ToLocalChecked()->ToObject();
  TypedArrays::Uint32Array.Reset(uint32Array);
  TypedArrays::Uint32Array_ctor.Reset(v8::Local<v8::Function>::Cast(uint32Array));
  TypedArrays::Uint32Array_from.Reset(v8::Local<v8::Function>::Cast(Nan::Get(uint32Array, v8::String::NewFromUtf8(isolate, "from")).ToLocalChecked()));

  auto buffer = Nan::Get(global, v8::String::NewFromUtf8(isolate, "Buffer")).ToLocalChecked()->ToObject();
  TypedArrays::Buffer.Reset(buffer);
  TypedArrays::Buffer_allocUnsafe.Reset(v8::Local<v8::Function>::Cast(Nan::Get(buffer, v8::String::NewFromUtf8(isolate, "allocUnsafe")).ToLocalChecked()));

  auto array = Nan::Get(global, v8::String::NewFromUtf8(isolate, "Array")).ToLocalChecked()->ToObject();
  TypedArrays::Array.Reset(array);
  TypedArrays::Array_from.Reset(v8::Local<v8::Function>::Cast(Nan::Get(array, v8::String::NewFromUtf8(isolate, "from")).ToLocalChecked()));

  auto set = Nan::Get(global, v8::String::NewFromUtf8(isolate, "Set")).ToLocalChecked()->ToObject();
  TypedArrays::Set.Reset(set);
  TypedArrays::Set_ctor.Reset(v8::Local<v8::Function>::Cast(set));
}

v8::Local<v8::Value> TypedArrays::bufferAllocUnsafe(v8::Isolate * isolate, size_t size) {
  v8::Local<v8::Value> argv[] = {v8::Number::New(isolate, (double)size)};
  return TypedArrays::Buffer_allocUnsafe.Get(isolate)->Call(TypedArrays::Uint32Array.Get(isolate), 1, argv);
}

namespace v8utils {

  void throwError(const char * message) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, message)));
  }

  void throwTypeError(const char * message) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, message)));
  }

}  // namespace v8utils
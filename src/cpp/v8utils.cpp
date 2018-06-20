#include "v8utils.h"

v8::Persistent<v8::Object> TypedArrays::Uint32Array;
v8::Persistent<v8::Function> TypedArrays::Uint32Array_ctor;
v8::Persistent<v8::Function> TypedArrays::Uint32Array_from;

v8::Persistent<v8::Object> TypedArrays::Buffer;
v8::Persistent<v8::Function> TypedArrays::Buffer_allocUnsafe;

v8::Persistent<v8::Object> TypedArrays::Array;
v8::Persistent<v8::Function> TypedArrays::Array_from;

v8::Persistent<v8::Object> TypedArrays::Set;
v8::Persistent<v8::Function> TypedArrays::Set_ctor;

void TypedArrays::initTypedArrays(v8::Isolate * isolate, const v8::Local<v8::Object> & global) {
  v8::HandleScope scope(isolate);

  auto uint32Array = global->Get(v8::String::NewFromUtf8(isolate, "Uint32Array"))->ToObject();
  TypedArrays::Uint32Array.Reset(isolate, uint32Array);
  TypedArrays::Uint32Array_ctor.Reset(isolate, v8::Local<v8::Function>::Cast(uint32Array));
  TypedArrays::Uint32Array_from.Reset(isolate, v8::Local<v8::Function>::Cast(uint32Array->Get(v8::String::NewFromUtf8(isolate, "from"))));

  auto buffer = global->Get(v8::String::NewFromUtf8(isolate, "Buffer"))->ToObject();
  TypedArrays::Buffer.Reset(isolate, buffer);
  TypedArrays::Buffer_allocUnsafe.Reset(isolate, v8::Local<v8::Function>::Cast(buffer->Get(v8::String::NewFromUtf8(isolate, "allocUnsafe"))));

  auto array = global->Get(v8::String::NewFromUtf8(isolate, "Array"))->ToObject();
  TypedArrays::Array.Reset(isolate, array);
  TypedArrays::Array_from.Reset(isolate, v8::Local<v8::Function>::Cast(array->Get(v8::String::NewFromUtf8(isolate, "from"))));

  auto set = global->Get(v8::String::NewFromUtf8(isolate, "Set"))->ToObject();
  TypedArrays::Set.Reset(isolate, set);
  TypedArrays::Set_ctor.Reset(isolate, v8::Local<v8::Function>::Cast(set));
}

v8::Local<v8::Value> TypedArrays::bufferAllocUnsafe(v8::Isolate * isolate, size_t size) {
  v8::Local<v8::Value> argv[] = {v8::Number::New(isolate, (double)size)};
  return TypedArrays::Buffer_allocUnsafe.Get(isolate)->Call(TypedArrays::Uint32Array.Get(isolate), 1, argv);
}

namespace v8utils {

  void throwError(const char * message) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, message)));
  }

  void throwTypeError(const char * message) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, message)));
  }

  void defineHiddenField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(false);
    target->DefineProperty(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name), propertyDescriptor).ToChecked();
  }

  void defineReadonlyField(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(true);
    target->DefineProperty(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name), propertyDescriptor).ToChecked();
  }

  void defineHiddenFunction(v8::Isolate * isolate, v8::Local<v8::Object> target, const char * name, v8::FunctionCallback callback) {
    v8::HandleScope scope(isolate);
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback);
    t->SetClassName(v8::String::NewFromUtf8(isolate, name));
    defineHiddenField(isolate, target, name, t->GetFunction());
  }

}  // namespace v8utils
#include "v8utils.h"

v8::Persistent<v8::Object> JSTypes::Uint32Array;
v8::Persistent<v8::Function> JSTypes::Uint32Array_ctor;
v8::Persistent<v8::Function> JSTypes::Uint32Array_from;

v8::Persistent<v8::Object> JSTypes::Buffer;
v8::Persistent<v8::Function> JSTypes::Buffer_allocUnsafe;

v8::Persistent<v8::Object> JSTypes::Array;
v8::Persistent<v8::Function> JSTypes::Array_from;

v8::Persistent<v8::Object> JSTypes::Set;
v8::Persistent<v8::Function> JSTypes::Set_ctor;

void JSTypes::initJSTypes(v8::Isolate * isolate, const v8::Local<v8::Object> & global) {
  v8::HandleScope scope(isolate);

  auto uint32Array = global->Get(v8::String::NewFromUtf8(isolate, "Uint32Array"))->ToObject();
  JSTypes::Uint32Array.Reset(isolate, uint32Array);
  JSTypes::Uint32Array_ctor.Reset(isolate, v8::Local<v8::Function>::Cast(uint32Array));
  JSTypes::Uint32Array_from.Reset(isolate, v8::Local<v8::Function>::Cast(uint32Array->Get(v8::String::NewFromUtf8(isolate, "from"))));

  auto buffer = global->Get(v8::String::NewFromUtf8(isolate, "Buffer"))->ToObject();
  JSTypes::Buffer.Reset(isolate, buffer);
  JSTypes::Buffer_allocUnsafe.Reset(isolate, v8::Local<v8::Function>::Cast(buffer->Get(v8::String::NewFromUtf8(isolate, "allocUnsafe"))));

  auto array = global->Get(v8::String::NewFromUtf8(isolate, "Array"))->ToObject();
  JSTypes::Array.Reset(isolate, array);
  JSTypes::Array_from.Reset(isolate, v8::Local<v8::Function>::Cast(array->Get(v8::String::NewFromUtf8(isolate, "from"))));

  auto set = global->Get(v8::String::NewFromUtf8(isolate, "Set"))->ToObject();
  JSTypes::Set.Reset(isolate, set);
  JSTypes::Set_ctor.Reset(isolate, v8::Local<v8::Function>::Cast(set));
}

v8::Local<v8::Value> JSTypes::bufferAllocUnsafe(v8::Isolate * isolate, size_t size) {
  v8::Local<v8::Value> argv[] = {v8::Number::New(isolate, (double)size)};
  return JSTypes::Buffer_allocUnsafe.Get(isolate)->Call(JSTypes::Uint32Array.Get(isolate), 1, argv);
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

  ObjectWrap::~ObjectWrap() {
    if (!persistent.IsEmpty()) {
      persistent.ClearWeak();
      persistent.Reset();
    }
  }

  void ObjectWrap::WeakCallback(v8::WeakCallbackInfo<ObjectWrap> const & info) {
    ObjectWrap * wrap = info.GetParameter();
    wrap->persistent.Reset();
    delete wrap;
  }

}  // namespace v8utils
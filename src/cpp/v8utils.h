#ifndef __V8UTILS__H__
#define __V8UTILS__H__

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

  static Nan::Persistent<v8::Object> Set;
  static Nan::Persistent<v8::Function> Set_ctor;

  static void initTypedArrays(v8::Isolate * isolate, const v8::Local<v8::Object> & global);

  static v8::Local<v8::Value> bufferAllocUnsafe(v8::Isolate * isolate, size_t size);
};

namespace v8utils {

  void throwError(const char * message);
  void throwTypeError(const char * message);

  template <typename T>
  void defineHiddenField(const T & target, const char * name, const v8::Local<v8::Value> & value) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(false);
    target->DefineProperty(Nan::GetCurrentContext(), v8::String::NewFromUtf8(isolate, name), propertyDescriptor).ToChecked();
  }

  template <typename T>
  void defineReadonlyField(const T & target, const char * name, const v8::Local<v8::Value> & value) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(true);
    target->DefineProperty(Nan::GetCurrentContext(), v8::String::NewFromUtf8(isolate, name), propertyDescriptor).ToChecked();
  }

  template <typename T, template <typename> class HandleType>
  void defineHiddenFunction(HandleType<T> recv, const char * name, v8::FunctionCallback callback) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback);
    t->SetClassName(v8::String::NewFromUtf8(isolate, name));
    defineHiddenField(recv, name, t->GetFunction());
  }

}  // namespace v8utils

#endif
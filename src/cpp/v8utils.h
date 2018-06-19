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

  static void initTypedArrays(const v8::Local<v8::Object> & global);

  static v8::Local<v8::Value> bufferAllocUnsafe(v8::Isolate * isolate, size_t size);
};

namespace v8utils {

  void throwError(const char * message);
  void throwTypeError(const char * message);

  template <typename T>
  void defineHiddenField(const T & target, const char * name, const v8::Local<v8::Value> & value) {
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(false);
    target->DefineProperty(Nan::GetCurrentContext(), Nan::New(name).ToLocalChecked(), propertyDescriptor).ToChecked();
  }

  template <typename T>
  void defineReadonlyField(const T & target, const char * name, const v8::Local<v8::Value> & value) {
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(true);
    target->DefineProperty(Nan::GetCurrentContext(), Nan::New(name).ToLocalChecked(), propertyDescriptor).ToChecked();
  }

  template <typename T, template <typename> class HandleType>
  inline void defineHiddenFunction(HandleType<T> recv, const char * name, Nan::FunctionCallback callback) {
    Nan::HandleScope scope;
    v8::Local<v8::FunctionTemplate> t = Nan::New<v8::FunctionTemplate>(callback);
    v8::Local<v8::String> fn_name = Nan::New(name).ToLocalChecked();
    t->SetClassName(fn_name);
    defineHiddenField(recv, name, t->GetFunction());
  }

}  // namespace v8utils

#endif
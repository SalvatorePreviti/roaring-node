#ifndef __V8UTILS__H__
#define __V8UTILS__H__

#include <nan.h>

namespace v8utils {

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
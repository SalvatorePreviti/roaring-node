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

}  // namespace v8utils

#endif
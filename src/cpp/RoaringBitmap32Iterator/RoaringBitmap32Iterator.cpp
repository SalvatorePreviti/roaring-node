#include "../v8utils.h"

#include "../RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32Iterator.h"

v8::Persistent<v8::FunctionTemplate> RoaringBitmap32Iterator::constructorTemplate;
v8::Persistent<v8::Function> RoaringBitmap32Iterator::constructor;

void RoaringBitmap32Iterator::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  auto className = v8::String::NewFromUtf8(isolate, "RoaringBitmap32Iterator");

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, RoaringBitmap32Iterator::New);
  constructorTemplate.Reset(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  Nan::SetNamedPropertyHandler(ctor->InstanceTemplate(), namedPropertyGetter);

  NODE_SET_PROTOTYPE_METHOD(ctor, "next", next);

  auto ctorFunction = ctor->GetFunction();
  auto ctorObject = ctorFunction->ToObject();
  v8utils::defineHiddenField(isolate, ctorObject, "default", ctorObject);

  constructor.Reset(isolate, ctorFunction);

  exports->Set(className, ctorFunction);
}

void RoaringBitmap32Iterator::New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = constructor.Get(isolate);
    if (info.Length() < 1) {
      auto v = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
      if (!v.IsEmpty())
        info.GetReturnValue().Set(v.ToLocalChecked());
    } else {
      v8::Local<v8::Value> argv[1] = {info[0]};
      auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
      if (!v.IsEmpty())
        info.GetReturnValue().Set(v.ToLocalChecked());
    }
    return;
  }

  RoaringBitmap32 * roaring = nullptr;

  if (info.Length() >= 1) {
    if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0])) {
      return v8utils::throwTypeError("RoaringBitmap32Iterator::ctor - argument must be of type RoaringBitmap32");
    }
    roaring = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  }

  RoaringBitmap32Iterator * instance = new RoaringBitmap32Iterator();
  if (!instance) {
    return v8utils::throwError("RoaringBitmap32Iterator::ctor - allocation failed");
  }

  auto holder = info.Holder();

  instance->Wrap(isolate, holder);

  if (roaring) {
    instance->roaring = roaring;
    instance->it.has_value = true;
    instance->bitmap.Reset(isolate, info[0]);
  }

  info.GetReturnValue().Set(holder);
}

void setReturnValueToIteratorResult(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Object> obj = v8::Object::New(isolate);
  obj->Set(v8::String::NewFromUtf8(isolate, "value"), v8::Undefined(isolate));
  obj->Set(v8::String::NewFromUtf8(isolate, "done"), v8::Boolean::New(isolate, true));
  info.GetReturnValue().Set(obj);
}

void setReturnValueToIteratorResult(const v8::FunctionCallbackInfo<v8::Value> & info, uint32_t value) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Object> obj = v8::Object::New(isolate);
  obj->Set(v8::String::NewFromUtf8(isolate, "value"), v8::Uint32::NewFromUnsigned(isolate, value));
  obj->Set(v8::String::NewFromUtf8(isolate, "done"), v8::Boolean::New(isolate, false));
  info.GetReturnValue().Set(obj);
}

void RoaringBitmap32Iterator::next(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32Iterator * instance = v8utils::ObjectWrap::Unwrap<RoaringBitmap32Iterator>(info.Holder());

  if (!instance || !instance->it.has_value) {
    return setReturnValueToIteratorResult(info);
  }

  if (instance->it.parent == nullptr) {
    roaring_init_iterator(&instance->roaring->roaring, &instance->it);
  } else if (!roaring_advance_uint32_iterator(&instance->it)) {
    instance->it.has_value = false;
    instance->bitmap.Reset();
    return setReturnValueToIteratorResult(info);
  }

  if (!instance->it.has_value) {
    instance->bitmap.Reset();
    return setReturnValueToIteratorResult(info);
  }

  return setReturnValueToIteratorResult(info, instance->it.current_value);
}

NAN_PROPERTY_GETTER(RoaringBitmap32Iterator::namedPropertyGetter) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (property->IsSymbol()) {
    if (property->Equals(v8::Symbol::GetIterator(isolate))) {
      RoaringBitmap32 * instance = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

      v8::Local<v8::FunctionTemplate> iterTemplate = v8::FunctionTemplate::New(isolate,
          [](const v8::FunctionCallbackInfo<v8::Value> & info) {
            v8::HandleScope scope(info.GetIsolate());
            info.GetReturnValue().Set(info.Holder());
          },
          v8::External::New(isolate, instance));

      info.GetReturnValue().Set(iterTemplate->GetFunction());
    }
  }
}

RoaringBitmap32Iterator::RoaringBitmap32Iterator() : roaring(nullptr) {
  this->it.parent = nullptr;
  this->it.has_value = false;
}

RoaringBitmap32Iterator::~RoaringBitmap32Iterator() {
  this->bitmap.Reset();
}

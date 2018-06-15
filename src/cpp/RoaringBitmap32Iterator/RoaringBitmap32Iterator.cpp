#include "../v8utils.h"

#include "../RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32Iterator.h"

Nan::Persistent<v8::FunctionTemplate> RoaringBitmap32Iterator::constructorTemplate;
Nan::Persistent<v8::Function> RoaringBitmap32Iterator::constructor;

void RoaringBitmap32Iterator::Init(v8::Local<v8::Object> exports) {
  auto className = Nan::New("RoaringBitmap32Iterator").ToLocalChecked();

  v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(RoaringBitmap32Iterator::New);
  constructorTemplate.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  Nan::SetNamedPropertyHandler(ctor->InstanceTemplate(), namedPropertyGetter);

  Nan::SetPrototypeMethod(ctor, "next", next);

  auto ctorFunction = ctor->GetFunction();
  auto ctorObject = ctorFunction->ToObject();
  v8utils::defineHiddenField(ctorObject, "default", ctorObject);

  constructor.Reset(ctorFunction);

  exports->Set(className, ctorFunction);
}

void RoaringBitmap32Iterator::New(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = Nan::New(constructor);
    if (info.Length() < 1) {
      auto v = Nan::NewInstance(cons, 0, nullptr);
      if (!v.IsEmpty())
        info.GetReturnValue().Set(v.ToLocalChecked());
    } else {
      v8::Local<v8::Value> argv[1] = {info[0]};
      auto v = Nan::NewInstance(cons, 1, argv);
      if (!v.IsEmpty())
        info.GetReturnValue().Set(v.ToLocalChecked());
    }
    return;
  }

  RoaringBitmap32 * roaring = nullptr;

  if (info.Length() >= 1) {
    if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0])) {
      return Nan::ThrowTypeError(Nan::New("RoaringBitmap32Iterator::ctor - argument must be of type RoaringBitmap32").ToLocalChecked());
    }
    roaring = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  }

  RoaringBitmap32Iterator * instance = new RoaringBitmap32Iterator();
  if (!instance) {
    return Nan::ThrowError(Nan::New("RoaringBitmap32Iterator::ctor - allocation failed").ToLocalChecked());
  }

  auto holder = info.Holder();

  instance->Wrap(holder);

  if (roaring) {
    instance->roaring = roaring;
    instance->it.has_value = true;
    instance->bitmap.Reset(info.GetIsolate(), info[0]);
  }

  info.GetReturnValue().Set(holder);
}

void setReturnValueToIteratorResult(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  auto obj = Nan::New<v8::Object>();
  obj->Set(Nan::New<v8::String>("value").ToLocalChecked(), Nan::Undefined());
  obj->Set(Nan::New<v8::String>("done").ToLocalChecked(), Nan::New<v8::Boolean>(true));
  info.GetReturnValue().Set(obj);
}

void setReturnValueToIteratorResult(const Nan::FunctionCallbackInfo<v8::Value> & info, uint32_t value) {
  auto obj = Nan::New<v8::Object>();
  obj->Set(Nan::New<v8::String>("value").ToLocalChecked(), Nan::New<v8::Uint32>(value));
  obj->Set(Nan::New<v8::String>("done").ToLocalChecked(), Nan::New<v8::Boolean>(false));
  info.GetReturnValue().Set(obj);
}

void RoaringBitmap32Iterator::next(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32Iterator * instance = Nan::ObjectWrap::Unwrap<RoaringBitmap32Iterator>(info.This());

  if (!instance || !instance->it.has_value) {
    return setReturnValueToIteratorResult(info);
  }

  if (instance->it.parent == nullptr) {
    roaring_init_iterator(&instance->roaring->roaring, &instance->it);
  } else {
    if (!roaring_advance_uint32_iterator(&instance->it)) {
      instance->it.has_value = false;
      instance->bitmap.Reset();
      return setReturnValueToIteratorResult(info);
    }
  }

  if (!instance->it.has_value) {
    instance->bitmap.Reset();
    return setReturnValueToIteratorResult(info);
  }

  return setReturnValueToIteratorResult(info, instance->it.current_value);
}

NAN_PROPERTY_GETTER(RoaringBitmap32Iterator::namedPropertyGetter) {
  if (property->IsSymbol()) {
    if (Nan::Equals(property, v8::Symbol::GetIterator(info.GetIsolate())).FromJust()) {
      auto instance = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.This());
      auto iter_template = Nan::New<v8::FunctionTemplate>();
      Nan::SetCallHandler(
          iter_template, [](const Nan::FunctionCallbackInfo<v8::Value> & info) { info.GetReturnValue().Set(info.This()); }, Nan::New<v8::External>(instance));
      info.GetReturnValue().Set(iter_template->GetFunction());
    }
  }
}

RoaringBitmap32Iterator::RoaringBitmap32Iterator() : roaring(nullptr) {
  this->it.parent = nullptr;
  this->it.has_value = false;
}

RoaringBitmap32Iterator::~RoaringBitmap32Iterator() {
}

#include "RoaringBitmap32Iterator.h"
#include "../RoaringBitmap32/RoaringBitmap32.h"

////////////// RoaringBitmap32IteratorState //////////////

v8::Persistent<v8::FunctionTemplate> RoaringBitmap32IteratorState::constructorTemplate;
v8::Persistent<v8::Function> RoaringBitmap32IteratorState::constructor;

RoaringBitmap32IteratorState::RoaringBitmap32IteratorState() : roaring(nullptr) {
  this->it.parent = nullptr;
  this->it.has_value = false;
}

void RoaringBitmap32IteratorState::Init(v8::Isolate * isolate) {
  v8::HandleScope scope(isolate);

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, New);
  constructorTemplate.Reset(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  v8::Local<v8::ObjectTemplate> ctorInstanceTemplate = ctor->InstanceTemplate();

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "done"), RoaringBitmap32IteratorState::done_getter, nullptr, v8::Local<v8::Value>(),
      (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "value"), RoaringBitmap32IteratorState::value_getter, nullptr, v8::Local<v8::Value>(),
      (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly));

  constructor.Reset(isolate, ctor->GetFunction());
}

void RoaringBitmap32IteratorState::New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    return;
  }

  auto holder = info.Holder();

  RoaringBitmap32 * roaring = nullptr;

  if (info.Length() >= 1) {
    if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0])) {
      return v8utils::throwTypeError("RoaringBitmap32Iterator::ctor - argument must be of type RoaringBitmap32");
    }
    roaring = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  }

  RoaringBitmap32IteratorState * instance = new RoaringBitmap32IteratorState();
  if (!instance) {
    return v8utils::throwError("RoaringBitmap32Iterator::ctor - allocation failed");
  }

  instance->Wrap(isolate, holder);

  if (roaring) {
    instance->roaring = roaring;
    instance->it.has_value = true;
  }

  info.GetReturnValue().Set(holder);
}

void RoaringBitmap32IteratorState::done_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  const RoaringBitmap32IteratorState * instance = v8utils::ObjectWrap::Unwrap<RoaringBitmap32IteratorState>(info.Holder());
  info.GetReturnValue().Set(!instance->it.has_value);
}

void RoaringBitmap32IteratorState::value_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32IteratorState * instance = v8utils::ObjectWrap::Unwrap<RoaringBitmap32IteratorState>(info.Holder());
  if (instance->it.has_value && instance->it.parent) {
    info.GetReturnValue().Set(instance->it.current_value);
  }
}

////////////// RoaringBitmap32Iterator //////////////

v8::Persistent<v8::FunctionTemplate> RoaringBitmap32Iterator::constructorTemplate;
v8::Persistent<v8::Function> RoaringBitmap32Iterator::constructor;

void RoaringBitmap32Iterator::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  RoaringBitmap32IteratorState::Init(isolate);

  v8::HandleScope scope(isolate);

  auto className = v8::String::NewFromUtf8(isolate, "RoaringBitmap32Iterator");

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, New);

  constructorTemplate.Reset(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  v8::Local<v8::ObjectTemplate> ctorInstanceTemplate = ctor->InstanceTemplate();

  ctorInstanceTemplate->SetAccessor(v8::Symbol::GetIterator(isolate), iterator_getter, nullptr, v8::Local<v8::Value>(),
      (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly | v8::DontEnum));

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "done"), RoaringBitmap32IteratorState::done_getter, nullptr, v8::Local<v8::Value>(),
      (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "value"), RoaringBitmap32IteratorState::value_getter, nullptr, v8::Local<v8::Value>(),
      (v8::AccessControl)(v8::DEFAULT), (v8::PropertyAttribute)(v8::ReadOnly));

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

  v8::Local<v8::Object> holder = info.Holder();

  v8::Local<v8::Function> stateCons = RoaringBitmap32IteratorState::constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> stateMaybe;

  if (info.Length() >= 1) {
    v8::Local<v8::Value> argv[1] = {info[0]};
    stateMaybe = stateCons->NewInstance(isolate->GetCurrentContext(), 1, argv);
  } else {
    stateMaybe = stateCons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  }

  if (stateMaybe.IsEmpty()) {
    return;
  }

  auto state = stateMaybe.ToLocalChecked();

  holder->SetAlignedPointerInInternalField(0, state->GetAlignedPointerFromInternalField(0));

  if (info.Length() >= 1) {
    v8utils::defineHiddenField(isolate, holder, "bitmap", info[0]);
  } else {
    v8utils::defineHiddenField(isolate, holder, "bitmap", v8::Undefined(isolate));
  }

  v8utils::defineHiddenField(isolate, holder, "current", state);

  info.GetReturnValue().Set(holder);
}

void RoaringBitmap32Iterator::next(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32IteratorState * instance = v8utils::ObjectWrap::Unwrap<RoaringBitmap32IteratorState>(info.Holder());

  if (instance->it.has_value) {
    if (instance->it.parent == nullptr) {
      roaring_init_iterator(&instance->roaring->roaring, &instance->it);
    } else if (!roaring_advance_uint32_iterator(&instance->it)) {
      instance->it.has_value = false;
    }
  }

  return info.GetReturnValue().Set(instance->persistent.Get(isolate));
}

void RoaringBitmap32Iterator::iterator_getter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::FunctionTemplate> iterTemplate = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::HandleScope scope(info.GetIsolate());
    info.GetReturnValue().Set(info.Holder());
  });

  info.GetReturnValue().Set(iterTemplate->GetFunction());
}

#include "v8utils.h"

#include "RoaringBitmap32.h"

Nan::Persistent<v8::FunctionTemplate> RoaringBitmap32::constructor;
Nan::Persistent<v8::Object> _Uint32Array;
Nan::Persistent<v8::Function> _Uint32Array_from;

static roaring_bitmap_t roaring_bitmap_zero;

NAN_MODULE_INIT(RoaringBitmap32::Init) {
  memset(&roaring_bitmap_zero, 0, sizeof(roaring_bitmap_zero));

  auto className = Nan::New("RoaringBitmap32").ToLocalChecked();

  v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(RoaringBitmap32::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  auto ctorFunction = ctor->GetFunction();
  auto ctorObject = ctorFunction->ToObject();
  auto ctorInstanceTemplate = ctor->InstanceTemplate();

  v8utils::defineHiddenField(ctorObject, "default", ctorObject);

  Nan::SetAccessor(ctorInstanceTemplate, Nan::New("isEmpty").ToLocalChecked(), isEmpty_getter);
  Nan::SetAccessor(ctorInstanceTemplate, Nan::New("size").ToLocalChecked(), size_getter);

  Nan::SetPrototypeMethod(ctor, "has", has);
  Nan::SetPrototypeMethod(ctor, "add", add);
  Nan::SetPrototypeMethod(ctor, "tryAdd", tryAdd);
  Nan::SetPrototypeMethod(ctor, "addMany", addMany);
  Nan::SetPrototypeMethod(ctor, "remove", remove);
  Nan::SetPrototypeMethod(ctor, "removeMany", removeMany);
  Nan::SetPrototypeMethod(ctor, "delete", removeChecked);
  Nan::SetPrototypeMethod(ctor, "clear", clear);

  Nan::SetPrototypeMethod(ctor, "orInPlace", addMany);
  Nan::SetPrototypeMethod(ctor, "andNotInPlace", removeMany);

  Nan::SetPrototypeMethod(ctor, "minimum", minimum);
  Nan::SetPrototypeMethod(ctor, "maximum", maximum);

  target->Set(className, ctorFunction);

  auto uint32ArrayType = Nan::Get(Nan::GetCurrentContext()->Global(), Nan::New("Uint32Array").ToLocalChecked()).ToLocalChecked()->ToObject();
  _Uint32Array.Reset(uint32ArrayType);
  _Uint32Array_from.Reset(v8::Local<v8::Function>::Cast(Nan::Get(uint32ArrayType, Nan::New("from").ToLocalChecked()).ToLocalChecked()));
}

RoaringBitmap32::RoaringBitmap32() : roaring(roaring_bitmap_zero) {
}

RoaringBitmap32::~RoaringBitmap32() {
  ra_clear(&roaring.high_low_container);
}

NAN_METHOD(RoaringBitmap32::New) {
  // throw an error if constructor is called without new keyword
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(Nan::New("RoaringBitmap32::New - called without new keyword").ToLocalChecked());
  }

  // create a new instance and wrap our javascript instance
  RoaringBitmap32 * instance = new RoaringBitmap32();

  if (!instance || !ra_init(&instance->roaring.high_low_container)) {
    if (instance != nullptr)
      delete instance;
    Nan::ThrowError(Nan::New("RoaringBitmap32::ctor - failed to initialize native roaring container").ToLocalChecked());
  }

  instance->Wrap(info.Holder());

  // return the wrapped javascript instance
  info.GetReturnValue().Set(info.Holder());

  if (info.Length() >= 1) {
    instance->addMany(info);
  }
}

NAN_PROPERTY_GETTER(RoaringBitmap32::size_getter) {
  const RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
  auto size = roaring_bitmap_get_cardinality(&self->roaring);
  if (size <= 0xFFFFFFFF) {
    info.GetReturnValue().Set((uint32_t)size);
  } else {
    info.GetReturnValue().Set((double)size);
  }
}

NAN_PROPERTY_GETTER(RoaringBitmap32::isEmpty_getter) {
  const RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set(roaring_bitmap_is_empty(&self->roaring));
}

NAN_METHOD(RoaringBitmap32::has) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    info.GetReturnValue().Set(false);
  } else {
    const RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
    info.GetReturnValue().Set(roaring_bitmap_contains(&self->roaring, info[0]->Uint32Value()));
  }
}

NAN_METHOD(RoaringBitmap32::minimum) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_minimum(&self->roaring));
}

NAN_METHOD(RoaringBitmap32::maximum) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_maximum(&self->roaring));
}

NAN_METHOD(RoaringBitmap32::add) {
  if (info.Length() < 1 || !info[0]->IsUint32())
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::add - 32 bit unsigned integer expected").ToLocalChecked());

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_bitmap_add(&self->roaring, info[0]->Uint32Value());
  return info.GetReturnValue().Set(info.This());
}

NAN_METHOD(RoaringBitmap32::tryAdd) {
  if (info.Length() < 1 || !info[0]->IsUint32())
    return info.GetReturnValue().Set(false);

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  auto oldSize = roaring_bitmap_get_cardinality(&self->roaring);
  roaring_bitmap_add(&self->roaring, info[0]->Uint32Value());
  return info.GetReturnValue().Set(oldSize != roaring_bitmap_get_cardinality(&self->roaring));
}

template <typename TArg>
void roaringAddMany(v8::Isolate * isolate, RoaringBitmap32 * self, const TArg & arg) {
  if (arg->IsUint32Array() || arg->IsInt32Array()) {
    Nan::TypedArrayContents<uint32_t> typedArray(arg);
    roaring_bitmap_add_many(&self->roaring, typedArray.length(), *typedArray);
    return;
  }

  if (arg->IsObject() && !arg->IsNull()) {
    if (RoaringBitmap32::constructor.Get(isolate)->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_or_inplace(&self->roaring, &other->roaring);
    } else {
      v8::Local<v8::Value> argv[] = {arg};
      Nan::TypedArrayContents<uint32_t> typedArray(_Uint32Array_from.Get(isolate)->Call(_Uint32Array.Get(isolate), 1, argv));
      roaring_bitmap_add_many(&self->roaring, typedArray.length(), *typedArray);
    }

    return;
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

NAN_METHOD(RoaringBitmap32::addMany) {
  if (info.Length() > 0) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaringAddMany(info.GetIsolate(), self, info[0]);
    return info.GetReturnValue().Set(info.This());
  }
  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

NAN_METHOD(RoaringBitmap32::removeMany) {
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (RoaringBitmap32::constructor.Get(info.GetIsolate())->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_andnot_inplace(&self->roaring, &other->roaring);
      return info.GetReturnValue().Set(info.This());
    } else {
      RoaringBitmap32 tmp;
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_andnot_inplace(&self->roaring, &tmp.roaring);
      return info.GetReturnValue().Set(info.This());
    }
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

NAN_METHOD(RoaringBitmap32::remove) {
  if (info.Length() >= 1 && info[0]->IsUint32()) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_remove(&self->roaring, info[0]->Uint32Value());
  }
}

NAN_METHOD(RoaringBitmap32::removeChecked) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    info.GetReturnValue().Set(false);
  } else {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    auto oldSize = roaring_bitmap_get_cardinality(&self->roaring);
    if (oldSize == 0) {
      info.GetReturnValue().Set(false);
    } else {
      roaring_bitmap_remove(&self->roaring, info[0]->Uint32Value());
      info.GetReturnValue().Set(oldSize != roaring_bitmap_get_cardinality(&self->roaring));
    }
  }
}

NAN_METHOD(RoaringBitmap32::clear) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_bitmap_t newRoaring;
  if (!ra_init(&newRoaring.high_low_container)) {
    Nan::ThrowError(Nan::New("RoaringBitmap32::clear - failed to initialize a new roaring container").ToLocalChecked());
  }
  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(newRoaring.high_low_container);
}

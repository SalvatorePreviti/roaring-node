#include "v8utils.h"

#include "RoaringBitmap32.h"
#include "RoaringBitmap32Iterator.h"

Nan::Persistent<v8::FunctionTemplate> RoaringBitmap32::constructorTemplate;
Nan::Persistent<v8::Function> RoaringBitmap32::constructor;
Nan::Persistent<v8::Object> _Uint32Array;
Nan::Persistent<v8::Function> _Uint32Array_from;

static roaring_bitmap_t roaring_bitmap_zero;

void RoaringBitmap32::Init(v8::Local<v8::Object> exports) {
  memset(&roaring_bitmap_zero, 0, sizeof(roaring_bitmap_zero));

  auto className = Nan::New("RoaringBitmap32").ToLocalChecked();

  v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(RoaringBitmap32::New);
  RoaringBitmap32::constructorTemplate.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  auto ctorInstanceTemplate = ctor->InstanceTemplate();

  Nan::SetAccessor(ctorInstanceTemplate, Nan::New("isEmpty").ToLocalChecked(), isEmpty_getter);
  Nan::SetAccessor(ctorInstanceTemplate, Nan::New("size").ToLocalChecked(), size_getter);

  Nan::SetNamedPropertyHandler(ctorInstanceTemplate, namedPropertyGetter);

  Nan::SetPrototypeMethod(ctor, "minimum", minimum);
  Nan::SetPrototypeMethod(ctor, "maximum", maximum);
  Nan::SetPrototypeMethod(ctor, "has", has);
  Nan::SetPrototypeMethod(ctor, "contains", has);
  Nan::SetPrototypeMethod(ctor, "add", add);
  Nan::SetPrototypeMethod(ctor, "tryAdd", tryAdd);
  Nan::SetPrototypeMethod(ctor, "addMany", addMany);
  Nan::SetPrototypeMethod(ctor, "remove", remove);
  Nan::SetPrototypeMethod(ctor, "removeMany", removeMany);
  Nan::SetPrototypeMethod(ctor, "delete", removeChecked);
  Nan::SetPrototypeMethod(ctor, "clear", clear);
  Nan::SetPrototypeMethod(ctor, "orInPlace", addMany);
  Nan::SetPrototypeMethod(ctor, "andNotInPlace", removeMany);
  Nan::SetPrototypeMethod(ctor, "isSubset", isSubset);
  Nan::SetPrototypeMethod(ctor, "isStrictSubset", isStrictSubset);

  auto ctorFunction = ctor->GetFunction();
  auto ctorObject = ctorFunction->ToObject();
  v8utils::defineHiddenField(ctorObject, "default", ctorObject);

  exports->Set(className, ctorFunction);
  constructor.Reset(ctorFunction);

  auto uint32ArrayType = Nan::Get(Nan::GetCurrentContext()->Global(), Nan::New("Uint32Array").ToLocalChecked()).ToLocalChecked()->ToObject();
  _Uint32Array.Reset(uint32ArrayType);
  _Uint32Array_from.Reset(v8::Local<v8::Function>::Cast(Nan::Get(uint32ArrayType, Nan::New("from").ToLocalChecked()).ToLocalChecked()));
}

RoaringBitmap32::RoaringBitmap32() : roaring(roaring_bitmap_zero) {
}

RoaringBitmap32::~RoaringBitmap32() {
  ra_clear(&roaring.high_low_container);
}

void RoaringBitmap32::New(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = Nan::New(constructor);
    if (info.Length() < 1) {
      v8::Local<v8::Value> argv[0] = {};
      info.GetReturnValue().Set(Nan::NewInstance(cons, 0, argv).ToLocalChecked());
    } else {
      v8::Local<v8::Value> argv[1] = {info[0]};
      info.GetReturnValue().Set(Nan::NewInstance(cons, 1, argv).ToLocalChecked());
    }
    return;
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

NAN_PROPERTY_GETTER(RoaringBitmap32::namedPropertyGetter) {
  if (property->IsSymbol()) {
    if (Nan::Equals(property, v8::Symbol::GetIterator(info.GetIsolate())).FromJust()) {
      auto self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.This());
      auto iter_template = Nan::New<v8::FunctionTemplate>();
      Nan::SetCallHandler(iter_template,
          [](const Nan::FunctionCallbackInfo<v8::Value> & info) {
            v8::Local<v8::Value> argv[1] = {info.This()};
            v8::Local<v8::Function> cons = Nan::New(RoaringBitmap32Iterator::constructor);
            info.GetReturnValue().Set(Nan::NewInstance(cons, 1, argv).ToLocalChecked());
          },
          Nan::New<v8::External>(self));
      info.GetReturnValue().Set(iter_template->GetFunction());
    }
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

void RoaringBitmap32::has(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    info.GetReturnValue().Set(false);
  } else {
    const RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
    info.GetReturnValue().Set(roaring_bitmap_contains(&self->roaring, info[0]->Uint32Value()));
  }
}

void RoaringBitmap32::minimum(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_minimum(&self->roaring));
}

void RoaringBitmap32::maximum(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_maximum(&self->roaring));
}

void RoaringBitmap32::add(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32())
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::add - 32 bit unsigned integer expected").ToLocalChecked());

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_bitmap_add(&self->roaring, info[0]->Uint32Value());
  return info.GetReturnValue().Set(info.This());
}

void RoaringBitmap32::tryAdd(const Nan::FunctionCallbackInfo<v8::Value> & info) {
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
    if (RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(arg)) {
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

void RoaringBitmap32::addMany(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaringAddMany(info.GetIsolate(), self, info[0]);
    return info.GetReturnValue().Set(info.This());
  }
  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::removeMany(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(arg)) {
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

void RoaringBitmap32::remove(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() >= 1 && info[0]->IsUint32()) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_remove(&self->roaring, info[0]->Uint32Value());
  }
}

void RoaringBitmap32::removeChecked(const Nan::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::clear(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_bitmap_t newRoaring;
  if (!ra_init(&newRoaring.high_low_container)) {
    Nan::ThrowError(Nan::New("RoaringBitmap32::clear - failed to initialize a new roaring container").ToLocalChecked());
  }
  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(newRoaring.high_low_container);
}

void RoaringBitmap32::isSubset(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0])) {
    return info.GetReturnValue().Set(false);
  }

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_is_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isStrictSubset(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0])) {
    return info.GetReturnValue().Set(false);
  }

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_is_strict_subset(&self->roaring, &other->roaring));
}

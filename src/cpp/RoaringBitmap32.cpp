#include "v8utils.h"

#include "RoaringBitmap32.h"
#include "RoaringBitmap32Iterator.h"
#include "TypedArrays.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

Nan::Persistent<v8::FunctionTemplate> RoaringBitmap32::constructorTemplate;
Nan::Persistent<v8::Function> RoaringBitmap32::constructor;

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
  Nan::SetPrototypeMethod(ctor, "andInPlace", andInPlace);
  Nan::SetPrototypeMethod(ctor, "xorInPlace", xorInPlace);
  Nan::SetPrototypeMethod(ctor, "isSubset", isSubset);
  Nan::SetPrototypeMethod(ctor, "isStrictSubset", isStrictSubset);
  Nan::SetPrototypeMethod(ctor, "isEqual", isEqual);
  Nan::SetPrototypeMethod(ctor, "intersects", intersects);
  Nan::SetPrototypeMethod(ctor, "andCardinality", andCardinality);
  Nan::SetPrototypeMethod(ctor, "orCardinality", orCardinality);
  Nan::SetPrototypeMethod(ctor, "andNotCardinality", andNotCardinality);
  Nan::SetPrototypeMethod(ctor, "xorCardinality", xorCardinality);
  Nan::SetPrototypeMethod(ctor, "jaccardIndex", jaccardIndex);
  Nan::SetPrototypeMethod(ctor, "flipRange", flipRange);
  Nan::SetPrototypeMethod(ctor, "removeRunCompression", removeRunCompression);
  Nan::SetPrototypeMethod(ctor, "runOptimize", runOptimize);
  Nan::SetPrototypeMethod(ctor, "shrinkToFit", shrinkToFit);
  Nan::SetPrototypeMethod(ctor, "rank", rank);
  Nan::SetPrototypeMethod(ctor, "toUint32Array", toUint32Array);
  Nan::SetPrototypeMethod(ctor, "getSerializationSizeInBytes", getSerializationSizeInBytes);
  Nan::SetPrototypeMethod(ctor, "serialize", serialize);

  auto ctorFunction = ctor->GetFunction();
  auto ctorObject = ctorFunction->ToObject();
  v8utils::defineHiddenField(ctorObject, "default", ctorObject);

  exports->Set(className, ctorFunction);
  constructor.Reset(ctorFunction);
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
      Nan::TypedArrayContents<uint32_t> typedArray(TypedArrays::Uint32Array_from.Get(isolate)->Call(TypedArrays::Uint32Array.Get(isolate), 1, argv));
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

void RoaringBitmap32::andInPlace(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_and_inplace(&self->roaring, &other->roaring);
      return info.GetReturnValue().Set(info.This());
    } else {
      RoaringBitmap32 tmp;
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_and_inplace(&self->roaring, &tmp.roaring);
      return info.GetReturnValue().Set(info.This());
    }
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::xorInPlace(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_xor_inplace(&self->roaring, &other->roaring);
      return info.GetReturnValue().Set(info.This());
    } else {
      RoaringBitmap32 tmp;
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_xor_inplace(&self->roaring, &tmp.roaring);
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
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(self == other || roaring_bitmap_is_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isStrictSubset(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_is_strict_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isEqual(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(self == other || roaring_bitmap_equals(&self->roaring, &other->roaring));
}

void RoaringBitmap32::intersects(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_intersect(&self->roaring, &other->roaring));
}

void RoaringBitmap32::andCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_and_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::orCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_or_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::andNotCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_andnot_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::xorCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_xor_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::jaccardIndex(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_jaccard_index(&self->roaring, &other->roaring));
}

void RoaringBitmap32::flipRange(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() >= 2 && info[0]->IsUint32() && info[1]->IsUint32()) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_flip_inplace(&self->roaring, info[0]->Uint32Value(), info[1]->Uint32Value());
  }
}

void RoaringBitmap32::removeRunCompression(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set(roaring_bitmap_remove_run_compression(&self->roaring));
}

void RoaringBitmap32::runOptimize(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set(roaring_bitmap_run_optimize(&self->roaring));
}

void RoaringBitmap32::shrinkToFit(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set((double)roaring_bitmap_shrink_to_fit(&self->roaring));
}

void RoaringBitmap32::rank(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() <= 1 || !info[0]->IsUint32()) {
    return info.GetReturnValue().Set(0);
  }

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set((double)roaring_bitmap_rank(&self->roaring, info[0]->Uint32Value()));
}

void RoaringBitmap32::toUint32Array(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  auto size = roaring_bitmap_get_cardinality(&self->roaring);

  if (size >= 0xFFFFFFFF) {
    Nan::ThrowError(Nan::New("RoaringBitmap32::toUint32Array - array too big").ToLocalChecked());
  }

  v8::Local<v8::Value> argv[1] = {Nan::New((uint32_t)size)};
  auto typedArray = Nan::NewInstance(TypedArrays::Uint32Array_ctor.Get(info.GetIsolate()), 1, argv).ToLocalChecked();

  if (size != 0) {
    Nan::TypedArrayContents<uint32_t> typedArrayContent(typedArray);
    if (!typedArrayContent.length() || !*typedArrayContent)
      Nan::ThrowError(Nan::New("RoaringBitmap32::toUint32Array - failed to allocate").ToLocalChecked());

    roaring_bitmap_to_uint32_array(&self->roaring, *typedArrayContent);
  }

  info.GetReturnValue().Set(typedArray);
}

void RoaringBitmap32::getSerializationSizeInBytes(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  bool portable = info.Length() > 0 && info[0]->IsTrue();

  auto portablesize = roaring_bitmap_portable_size_in_bytes(&self->roaring);

  if (portable) {
    return info.GetReturnValue().Set((double)portablesize);
  }

  auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
  auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

  if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
    return info.GetReturnValue().Set((double)(portablesize + 1));
  }

  return info.GetReturnValue().Set((double)(sizeasarray + 1));
}

void RoaringBitmap32::serialize(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  bool portable = info.Length() > 0 && info[0]->IsTrue();
  auto portablesize = roaring_bitmap_portable_size_in_bytes(&self->roaring);

  if (portable) {
    v8::Local<v8::Value> argv[1] = {Nan::New((double)(portablesize))};
    auto typedArray = Nan::NewInstance(TypedArrays::Uint8Array_ctor.Get(info.GetIsolate()), 1, argv).ToLocalChecked();
    Nan::TypedArrayContents<uint8_t> buf(typedArray);
    if (!buf.length() || !*buf)
      Nan::ThrowError(Nan::New("RoaringBitmap32::serialize - failed to allocate").ToLocalChecked());

    roaring_bitmap_portable_serialize(&self->roaring, (char *)*buf);
    info.GetReturnValue().Set(typedArray);
  } else {
    auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
    auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

    if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
      v8::Local<v8::Value> argv[1] = {Nan::New((double)(portablesize + 1))};
      auto typedArray = Nan::NewInstance(TypedArrays::Uint8Array_ctor.Get(info.GetIsolate()), 1, argv).ToLocalChecked();
      Nan::TypedArrayContents<uint8_t> buf(typedArray);
      if (!buf.length() || !*buf)
        Nan::ThrowError(Nan::New("RoaringBitmap32::serialize - failed to allocate").ToLocalChecked());

      (*buf)[0] = SERIALIZATION_CONTAINER;
      roaring_bitmap_portable_serialize(&self->roaring, (char *)*buf + 1);
      info.GetReturnValue().Set(typedArray);
    } else {
      v8::Local<v8::Value> argv[1] = {Nan::New((double)(sizeasarray + 1))};
      auto typedArray = Nan::NewInstance(TypedArrays::Uint8Array_ctor.Get(info.GetIsolate()), 1, argv).ToLocalChecked();
      Nan::TypedArrayContents<uint8_t> buf(typedArray);
      if (!buf.length() || !*buf)
        Nan::ThrowError(Nan::New("RoaringBitmap32::serialize - failed to allocate").ToLocalChecked());

      (*buf)[0] = SERIALIZATION_ARRAY_UINT32;
      memcpy(*buf + 1, &cardinality, sizeof(uint32_t));
      roaring_bitmap_to_uint32_array(&self->roaring, (uint32_t *)(*buf + 1 + sizeof(uint32_t)));
      info.GetReturnValue().Set(typedArray);
    }
  }
}
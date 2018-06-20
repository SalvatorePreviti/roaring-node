#include "../v8utils.h"

#include "../RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"
#include "RoaringBitmap32.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

v8::Persistent<v8::FunctionTemplate> RoaringBitmap32::constructorTemplate;
v8::Persistent<v8::Function> RoaringBitmap32::constructor;

uint8_t roaring_bitmap_zero[sizeof(roaring_bitmap_t)] = {0};

void RoaringBitmap32::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  v8::Local<v8::String> className = v8::String::NewFromUtf8(isolate, "RoaringBitmap32");

  v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(RoaringBitmap32::New);
  RoaringBitmap32::constructorTemplate.Reset(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  auto ctorInstanceTemplate = ctor->InstanceTemplate();

  Nan::SetAccessor(ctorInstanceTemplate, v8::String::NewFromUtf8(isolate, "isEmpty"), isEmpty_getter);
  Nan::SetAccessor(ctorInstanceTemplate, v8::String::NewFromUtf8(isolate, "size"), size_getter);

  Nan::SetNamedPropertyHandler(ctorInstanceTemplate, namedPropertyGetter);

  Nan::SetPrototypeMethod(ctor, "minimum", minimum);
  Nan::SetPrototypeMethod(ctor, "maximum", maximum);
  Nan::SetPrototypeMethod(ctor, "contains", has);
  Nan::SetPrototypeMethod(ctor, "has", has);
  Nan::SetPrototypeMethod(ctor, "containsRange", hasRange);
  Nan::SetPrototypeMethod(ctor, "hasRange", hasRange);
  Nan::SetPrototypeMethod(ctor, "copyFrom", copyFrom);
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
  Nan::SetPrototypeMethod(ctor, "addRange", addRange);
  Nan::SetPrototypeMethod(ctor, "removeRunCompression", removeRunCompression);
  Nan::SetPrototypeMethod(ctor, "runOptimize", runOptimize);
  Nan::SetPrototypeMethod(ctor, "shrinkToFit", shrinkToFit);
  Nan::SetPrototypeMethod(ctor, "rank", rank);
  Nan::SetPrototypeMethod(ctor, "select", select);
  Nan::SetPrototypeMethod(ctor, "toUint32Array", toUint32Array);
  Nan::SetPrototypeMethod(ctor, "toArray", toArray);
  Nan::SetPrototypeMethod(ctor, "toSet", toSet);
  Nan::SetPrototypeMethod(ctor, "toJSON", toArray);
  Nan::SetPrototypeMethod(ctor, "getSerializationSizeInBytes", getSerializationSizeInBytes);
  Nan::SetPrototypeMethod(ctor, "serialize", serialize);
  Nan::SetPrototypeMethod(ctor, "deserialize", deserialize);
  Nan::SetPrototypeMethod(ctor, "clone", clone);
  Nan::SetPrototypeMethod(ctor, "toString", toString);
  Nan::SetPrototypeMethod(ctor, "contentToString", contentToString);
  Nan::SetPrototypeMethod(ctor, "statistics", statistics);

  auto ctorFunction = ctor->GetFunction();
  auto ctorObject = ctorFunction->ToObject();

  Nan::SetMethod(ctorObject, "deserialize", deserializeStatic);
  Nan::SetMethod(ctorObject, "and", andStatic);
  Nan::SetMethod(ctorObject, "or", orStatic);
  Nan::SetMethod(ctorObject, "xor", xorStatic);
  Nan::SetMethod(ctorObject, "andNot", andNotStatic);
  Nan::SetMethod(ctorObject, "orMany", orManyStatic);

  NODE_SET_METHOD(ctorObject, "swap", swapStatic);

  ctorObject->Set(v8::String::NewFromUtf8(isolate, "from"), ctorFunction);

  v8utils::defineHiddenField(ctorObject, "default", ctorObject);

  exports->Set(className, ctorFunction);
  constructor.Reset(isolate, ctorFunction);
}

RoaringBitmap32::RoaringBitmap32() : roaring(*((roaring_bitmap_t *)&roaring_bitmap_zero)) {
}

RoaringBitmap32::~RoaringBitmap32() {
  ra_clear(&roaring.high_low_container);
}

void RoaringBitmap32::New(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = constructor.Get(isolate);
    if (info.Length() < 1) {
      auto v = Nan::NewInstance(cons, 0, nullptr);
      if (!v.IsEmpty()) {
        info.GetReturnValue().Set(v.ToLocalChecked());
      }
    } else {
      v8::Local<v8::Value> argv[1] = {info[0]};
      auto v = Nan::NewInstance(cons, 1, argv);
      if (!v.IsEmpty()) {
        info.GetReturnValue().Set(v.ToLocalChecked());
      }
    }
    return;
  }

  // create a new instance and wrap our javascript instance
  RoaringBitmap32 * instance = new RoaringBitmap32();

  if (!instance)
    return v8utils::throwError("RoaringBitmap32::ctor - failed to create native roaring container");

  bool hasParameter = info.Length() != 0 && !info[0]->IsUndefined() && !info[0]->IsNull();

  if (!hasParameter) {
    if (!ra_init(&instance->roaring.high_low_container)) {
      delete instance;
      return v8utils::throwError("RoaringBitmap32::ctor - failed to initialize native roaring container");
    }
  }

  instance->Wrap(info.Holder());

  // return the wrapped javascript instance
  info.GetReturnValue().Set(info.Holder());

  if (hasParameter) {
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0])) {
      instance->copyFrom(info);
    } else {
      instance->addMany(info);
    }
  }
}

NAN_PROPERTY_GETTER(RoaringBitmap32::namedPropertyGetter) {
  v8::Isolate * isolate = info.GetIsolate();

  if (property->IsSymbol()) {
    if (Nan::Equals(property, v8::Symbol::GetIterator(isolate)).FromJust()) {
      auto self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.This());

      v8::Local<v8::FunctionTemplate> iterTemplate = v8::FunctionTemplate::New(isolate,
          [](const v8::FunctionCallbackInfo<v8::Value> & info) {
            v8::Isolate * isolate = info.GetIsolate();
            v8::HandleScope scope(isolate);
            v8::Local<v8::Function> cons = RoaringBitmap32Iterator::constructor.Get(isolate);
            v8::Local<v8::Value> argv[1] = {info.This()};
            auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
            if (!resultMaybe.IsEmpty())
              info.GetReturnValue().Set(resultMaybe.ToLocalChecked());
          },
          v8::External::New(isolate, self));

      info.GetReturnValue().Set(iterTemplate->GetFunction());
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

void RoaringBitmap32::hasRange(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return info.GetReturnValue().Set(false);
  }

  double minimum = info[0]->NumberValue();
  double maximum = info[1]->NumberValue();

  if (std::isnan(minimum) || std::isnan(maximum)) {
    return info.GetReturnValue().Set(false);
  }

  minimum = std::ceil(minimum);
  maximum = std::ceil(maximum);
  if (minimum < 0 || maximum > 4294967296) {
    return info.GetReturnValue().Set(false);
  }

  uint64_t minInteger = (uint64_t)minimum;
  uint64_t maxInteger = (uint64_t)maximum;

  if (minInteger >= maxInteger || maxInteger > 4294967296) {
    return info.GetReturnValue().Set(false);
  }

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  info.GetReturnValue().Set(roaring_bitmap_contains_range(&self->roaring, minInteger, maxInteger));
}

void RoaringBitmap32::minimum(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_minimum(&self->roaring));
}

void RoaringBitmap32::maximum(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_maximum(&self->roaring));
}

void RoaringBitmap32::rank(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    return info.GetReturnValue().Set(0);
  }

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set((double)roaring_bitmap_rank(&self->roaring, info[0]->Uint32Value()));
}

void RoaringBitmap32::select(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    return info.GetReturnValue().Set(Nan::Undefined());
  }

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  uint32_t element;
  if (roaring_bitmap_select(&self->roaring, info[0]->Uint32Value(), &element)) {
    return info.GetReturnValue().Set(element);
  }

  return info.GetReturnValue().Set(Nan::Undefined());
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

void RoaringBitmap32::toUint32Array(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  auto size = roaring_bitmap_get_cardinality(&self->roaring);

  if (size >= 0xFFFFFFFF) {
    return v8utils::throwError("RoaringBitmap32::toUint32Array - array too big");
  }

  v8::Local<v8::Value> argv[1] = {Nan::New((uint32_t)size)};
  auto typedArrayMaybe = Nan::NewInstance(TypedArrays::Uint32Array_ctor.Get(info.GetIsolate()), 1, argv);
  if (typedArrayMaybe.IsEmpty())
    return;

  auto typedArray = typedArrayMaybe.ToLocalChecked();

  if (size != 0) {
    Nan::TypedArrayContents<uint32_t> typedArrayContent(typedArray);
    if (!typedArrayContent.length() || !*typedArrayContent)
      return v8utils::throwError("RoaringBitmap32::toUint32Array - failed to allocate");

    roaring_bitmap_to_uint32_array(&self->roaring, *typedArrayContent);
  }

  info.GetReturnValue().Set(typedArray);
}

void RoaringBitmap32::toArray(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  v8::Local<v8::Value> argv[1] = {info.Holder()};
  info.GetReturnValue().Set(TypedArrays::Array_from.Get(info.GetIsolate())->Call(TypedArrays::Array.Get(info.GetIsolate()), 1, argv));
}

void RoaringBitmap32::toSet(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  v8::Local<v8::Value> argv[1] = {info.This()};
  auto v = Nan::NewInstance(TypedArrays::Set_ctor.Get(info.GetIsolate()), 1, argv);
  if (!v.IsEmpty()) {
    info.GetReturnValue().Set(v.ToLocalChecked());
  }
}

void RoaringBitmap32::toString(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  std::string result("RoaringBitmap32:");
  result += std::to_string(self ? roaring_bitmap_get_cardinality(&self->roaring) : 0);
  info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, result.c_str()));
}

void RoaringBitmap32::contentToString(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  struct iter_data {
    std::string str;
    char first_char = '[';
    uint64_t maxLen = 32000;
  } iterData;

  if (info.Length() >= 1 && info[0]->IsNumber()) {
    double nv = info[0]->NumberValue();
    if (nv >= 0) {
      iterData.maxLen = (uint64_t)nv;
    }
  }

  if (self && !roaring_bitmap_is_empty(&self->roaring)) {
    roaring_iterate(&self->roaring,
        [](uint32_t value, void * p) -> bool {
          if (((iter_data *)p)->str.length() >= ((iter_data *)p)->maxLen) {
            ((iter_data *)p)->str.append("...");
            return false;
          }

          ((iter_data *)p)->str += ((iter_data *)p)->first_char;
          ((iter_data *)p)->str.append(std::to_string(value));
          ((iter_data *)p)->first_char = ',';
          return true;
        },
        (void *)&iterData);
  } else {
    iterData.str = '[';
  }
  iterData.str += ']';

  info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, iterData.str.c_str()));
}

void RoaringBitmap32::clone(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::Local<v8::Function> cons = constructor.Get(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto v = Nan::NewInstance(cons, 1, argv);
  if (!v.IsEmpty()) {
    info.GetReturnValue().Set(v.ToLocalChecked());
  }
}

void RoaringBitmap32::statistics(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_statistics_t stats;
  roaring_bitmap_statistics(&self->roaring, &stats);
  auto result = Nan::New<v8::Object>();
  result->Set(v8::String::NewFromUtf8(isolate, "containers"), Nan::New(stats.n_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "arrayContainers"), Nan::New(stats.n_array_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "runContainers"), Nan::New(stats.n_run_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bitsetContainers"), Nan::New(stats.n_bitset_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "valuesInArrayContainers"), Nan::New(stats.n_values_array_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "valuesInRunContainers"), Nan::New(stats.n_values_run_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "valuesInBitsetContainers"), Nan::New(stats.n_values_bitset_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bytesInArrayContainers"), Nan::New(stats.n_bytes_array_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bytesInRunContainers"), Nan::New(stats.n_bytes_run_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bytesInBitsetContainers"), Nan::New(stats.n_bytes_bitset_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "maxValue"), Nan::New(stats.max_value));
  result->Set(v8::String::NewFromUtf8(isolate, "minValue"), Nan::New(stats.min_value));
  result->Set(v8::String::NewFromUtf8(isolate, "sumOfAllValues"), v8::Number::New(isolate, (double)stats.sum_value));
  result->Set(v8::String::NewFromUtf8(isolate, "size"), v8::Number::New(isolate, (double)stats.cardinality));
  info.GetReturnValue().Set(result);
}

#include "../v8utils/v8utils.h"

#include "RoaringBitmap32.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

v8::Persistent<v8::FunctionTemplate> RoaringBitmap32::constructorTemplate;
v8::Persistent<v8::Function> RoaringBitmap32::constructor;

const uint8_t RoaringBitmap32::roaring_bitmap_zero[sizeof(roaring_bitmap_t)] = {0};

void RoaringBitmap32::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::Local<v8::String> className = v8::String::NewFromUtf8(isolate, "RoaringBitmap32");

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, RoaringBitmap32::New);
  RoaringBitmap32::constructorTemplate.Reset(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  v8::Local<v8::ObjectTemplate> ctorInstanceTemplate = ctor->InstanceTemplate();

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "isEmpty"), isEmpty_getter, nullptr, v8::Local<v8::Value>(),
      (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "size"), size_getter, nullptr, v8::Local<v8::Value>(),
      (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly));

  NODE_SET_PROTOTYPE_METHOD(ctor, "minimum", minimum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "maximum", maximum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contains", has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "has", has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "containsRange", hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "hasRange", hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "copyFrom", copyFrom);
  NODE_SET_PROTOTYPE_METHOD(ctor, "add", add);
  NODE_SET_PROTOTYPE_METHOD(ctor, "tryAdd", tryAdd);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addMany", addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "remove", remove);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeMany", removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "delete", removeChecked);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clear", clear);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orInPlace", addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotInPlace", removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andInPlace", andInPlace);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorInPlace", xorInPlace);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isSubset", isSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isStrictSubset", isStrictSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isEqual", isEqual);
  NODE_SET_PROTOTYPE_METHOD(ctor, "intersects", intersects);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andCardinality", andCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orCardinality", orCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotCardinality", andNotCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorCardinality", xorCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "jaccardIndex", jaccardIndex);
  NODE_SET_PROTOTYPE_METHOD(ctor, "flipRange", flipRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addRange", addRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRunCompression", removeRunCompression);
  NODE_SET_PROTOTYPE_METHOD(ctor, "runOptimize", runOptimize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "shrinkToFit", shrinkToFit);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rank", rank);
  NODE_SET_PROTOTYPE_METHOD(ctor, "select", select);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toUint32Array", toUint32Array);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toArray", toArray);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toSet", toSet);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toJSON", toArray);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSerializationSizeInBytes", getSerializationSizeInBytes);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serialize", serialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "deserialize", deserialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clone", clone);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toString", toString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contentToString", contentToString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "statistics", statistics);

  auto ctorFunction = ctor->GetFunction();
  auto ctorObject = ctorFunction->ToObject();

  NODE_SET_METHOD(ctorObject, "fromArrayAsync", fromArrayStaticAsync);
  NODE_SET_METHOD(ctorObject, "deserialize", deserializeStatic);
  NODE_SET_METHOD(ctorObject, "deserializeAsync", deserializeStaticAsync);
  NODE_SET_METHOD(ctorObject, "and", andStatic);
  NODE_SET_METHOD(ctorObject, "or", orStatic);
  NODE_SET_METHOD(ctorObject, "xor", xorStatic);
  NODE_SET_METHOD(ctorObject, "andNot", andNotStatic);
  NODE_SET_METHOD(ctorObject, "orMany", orManyStatic);
  NODE_SET_METHOD(ctorObject, "swap", swapStatic);

  ctorObject->Set(v8::String::NewFromUtf8(isolate, "from"), ctorFunction);

  v8utils::defineHiddenField(isolate, ctorObject, "default", ctorFunction);
  v8utils::defineHiddenField(isolate, ctorObject, "RoaringBitmap32", ctorFunction);

  exports->Set(className, ctorFunction);
  constructor.Reset(isolate, ctorFunction);
}

RoaringBitmap32::RoaringBitmap32() : roaring(*((roaring_bitmap_t *)&RoaringBitmap32::roaring_bitmap_zero)) {
}

RoaringBitmap32::~RoaringBitmap32() {
  ra_clear(&roaring.high_low_container);
}

void RoaringBitmap32::New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = constructor.Get(isolate);
    if (info.Length() < 1) {
      v8::MaybeLocal<v8::Object> v = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
      if (!v.IsEmpty()) {
        v8::Local<v8::Value> returnValue = v.ToLocalChecked();
        info.GetReturnValue().Set(scope.Escape(returnValue));
      }
    } else {
      v8::Local<v8::Value> argv[1] = {info[0]};
      auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
      if (!v.IsEmpty()) {
        v8::Local<v8::Value> returnValue = v.ToLocalChecked();
        info.GetReturnValue().Set(scope.Escape(returnValue));
      }
    }
    return;
  }

  RoaringBitmap32 * instance = new RoaringBitmap32();

  if (!instance)
    return v8utils::throwError("RoaringBitmap32::ctor - failed to create native roaring container");

  bool hasParameter = info.Length() != 0 && !info[0]->IsUndefined() && !info[0]->IsNull();

  auto holder = info.Holder();
  instance->Wrap(isolate, holder);

  if (hasParameter) {
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0])) {
      instance->copyFrom(info);
    } else {
      instance->addMany(info);
    }
  }

  info.GetReturnValue().Set(scope.Escape(holder));
}

void RoaringBitmap32::size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
  if (!self) {
    return info.GetReturnValue().Set(0U);
  }

  size_t size = roaring_bitmap_get_cardinality(&self->roaring);
  if (size <= 0xFFFFFFFF) {
    return info.GetReturnValue().Set((uint32_t)size);
  }

  info.GetReturnValue().Set((double)size);
}

void RoaringBitmap32::isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set(self && roaring_bitmap_is_empty(&self->roaring));
}

void RoaringBitmap32::has(const v8::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    info.GetReturnValue().Set(false);
  } else {
    const RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
    info.GetReturnValue().Set(roaring_bitmap_contains(&self->roaring, info[0]->Uint32Value()));
  }
}

void RoaringBitmap32::hasRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  info.GetReturnValue().Set(roaring_bitmap_contains_range(&self->roaring, minInteger, maxInteger));
}

void RoaringBitmap32::minimum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_minimum(&self->roaring));
}

void RoaringBitmap32::maximum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  return info.GetReturnValue().Set(roaring_bitmap_maximum(&self->roaring));
}

void RoaringBitmap32::rank(const v8::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    return info.GetReturnValue().Set(0);
  }

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set((double)roaring_bitmap_rank(&self->roaring, info[0]->Uint32Value()));
}

void RoaringBitmap32::select(const v8::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    return;
  }

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  uint32_t element;
  if (roaring_bitmap_select(&self->roaring, info[0]->Uint32Value(), &element)) {
    return info.GetReturnValue().Set(element);
  }
}

void RoaringBitmap32::removeRunCompression(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set(roaring_bitmap_remove_run_compression(&self->roaring));
}

void RoaringBitmap32::runOptimize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set(roaring_bitmap_run_optimize(&self->roaring));
}

void RoaringBitmap32::shrinkToFit(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set((double)roaring_bitmap_shrink_to_fit(&self->roaring));
}

void RoaringBitmap32::toUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  auto size = roaring_bitmap_get_cardinality(&self->roaring);

  if (size >= 0xFFFFFFFF) {
    return v8utils::throwError("RoaringBitmap32::toUint32Array - array too big");
  }

  v8::Local<v8::Value> argv[1] = {v8::Uint32::NewFromUnsigned(isolate, (uint32_t)size)};
  auto typedArrayMaybe = JSTypes::Uint32Array_ctor.Get(isolate)->NewInstance(isolate->GetCurrentContext(), 1, argv);
  if (typedArrayMaybe.IsEmpty())
    return;

  auto typedArray = typedArrayMaybe.ToLocalChecked();

  if (size != 0) {
    const v8utils::TypedArrayContent<uint32_t> typedArrayContent(typedArray);
    if (!typedArrayContent.length || !typedArrayContent.data)
      return v8utils::throwError("RoaringBitmap32::toUint32Array - failed to allocate");

    roaring_bitmap_to_uint32_array(&self->roaring, typedArrayContent.data);
  }

  info.GetReturnValue().Set(scope.Escape(typedArray));
}

void RoaringBitmap32::toArray(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  v8::Local<v8::Value> returnValue = JSTypes::Array_from.Get(info.GetIsolate())->Call(JSTypes::Array.Get(info.GetIsolate()), 1, argv);
  info.GetReturnValue().Set(scope.Escape(returnValue));
}

void RoaringBitmap32::toSet(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto v = JSTypes::Set_ctor.Get(isolate)->NewInstance(isolate->GetCurrentContext(), 1, argv);
  if (!v.IsEmpty()) {
    v8::Local<v8::Value> returnValue = v.ToLocalChecked();
    info.GetReturnValue().Set(scope.Escape(returnValue));
  }
}

void RoaringBitmap32::toString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  std::string result("RoaringBitmap32:");
  result += std::to_string(self ? roaring_bitmap_get_cardinality(&self->roaring) : 0);
  v8::Local<v8::Value> returnValue = (v8::String::NewFromUtf8(isolate, result.c_str()));
  info.GetReturnValue().Set(scope.Escape(returnValue));
}

void RoaringBitmap32::contentToString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
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

  v8::Local<v8::Value> returnValue = v8::String::NewFromUtf8(isolate, iterData.str.c_str());
  info.GetReturnValue().Set(scope.Escape(returnValue));
}

void RoaringBitmap32::clone(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
  if (!v.IsEmpty()) {
    v8::Local<v8::Value> returnValue = v.ToLocalChecked();
    info.GetReturnValue().Set(scope.Escape(returnValue));
  }
}

void RoaringBitmap32::statistics(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_statistics_t stats;
  roaring_bitmap_statistics(&self->roaring, &stats);
  auto result = v8::Object::New(isolate);
  result->Set(v8::String::NewFromUtf8(isolate, "containers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "arrayContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_array_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "runContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_run_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bitsetContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_bitset_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "valuesInArrayContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_values_array_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "valuesInRunContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_values_run_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "valuesInBitsetContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_values_bitset_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bytesInArrayContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_array_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bytesInRunContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_run_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "bytesInBitsetContainers"), v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_bitset_containers));
  result->Set(v8::String::NewFromUtf8(isolate, "maxValue"), v8::Uint32::NewFromUnsigned(isolate, stats.max_value));
  result->Set(v8::String::NewFromUtf8(isolate, "minValue"), v8::Uint32::NewFromUnsigned(isolate, stats.min_value));
  result->Set(v8::String::NewFromUtf8(isolate, "sumOfAllValues"), v8::Number::New(isolate, (double)stats.sum_value));
  result->Set(v8::String::NewFromUtf8(isolate, "size"), v8::Number::New(isolate, (double)stats.cardinality));
  info.GetReturnValue().Set(scope.Escape(result));
}

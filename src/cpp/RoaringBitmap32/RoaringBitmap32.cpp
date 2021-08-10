#include <iostream>
#include "../v8utils/v8utils.h"

#include "RoaringBitmap32.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

//////////// RoaringBitmap32 ////////////

v8::Eternal<v8::FunctionTemplate> RoaringBitmap32::constructorTemplate;
v8::Eternal<v8::Function> RoaringBitmap32::constructor;

void RoaringBitmap32::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::Local<v8::String> className = v8::String::NewFromUtf8(isolate, "RoaringBitmap32", v8::NewStringType::kInternalized).ToLocalChecked();

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, RoaringBitmap32::New);
  RoaringBitmap32::constructorTemplate.Set(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  v8::Local<v8::ObjectTemplate> ctorInstanceTemplate = ctor->InstanceTemplate();

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "isEmpty", v8::NewStringType::kInternalized).ToLocalChecked(), isEmpty_getter, nullptr,
      v8::Local<v8::Value>(), (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "size", v8::NewStringType::kInternalized).ToLocalChecked(), size_getter, nullptr,
      v8::Local<v8::Value>(), (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING), (v8::PropertyAttribute)(v8::ReadOnly));

  NODE_SET_PROTOTYPE_METHOD(ctor, "minimum", minimum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "maximum", maximum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contains", has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "has", has);
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

  NODE_SET_PROTOTYPE_METHOD(ctor, "containsRange", hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "hasRange", hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rangeCardinality", rangeCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "flipRange", flipRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addRange", addRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRange", removeRange);

  auto context = isolate->GetCurrentContext();
  auto ctorFunction = ctor->GetFunction(context).ToLocalChecked();
  auto ctorObject = ctorFunction->ToObject(context).ToLocalChecked();

  NODE_SET_METHOD(ctorObject, "fromRange", fromRangeStatic);
  NODE_SET_METHOD(ctorObject, "fromArrayAsync", fromArrayStaticAsync);
  NODE_SET_METHOD(ctorObject, "deserialize", deserializeStatic);
  NODE_SET_METHOD(ctorObject, "deserializeAsync", deserializeStaticAsync);
  NODE_SET_METHOD(ctorObject, "deserializeParallelAsync", deserializeParallelStaticAsync);
  NODE_SET_METHOD(ctorObject, "and", andStatic);
  NODE_SET_METHOD(ctorObject, "or", orStatic);
  NODE_SET_METHOD(ctorObject, "xor", xorStatic);
  NODE_SET_METHOD(ctorObject, "andNot", andNotStatic);
  NODE_SET_METHOD(ctorObject, "orMany", orManyStatic);
  NODE_SET_METHOD(ctorObject, "xorMany", xorManyStatic);
  NODE_SET_METHOD(ctorObject, "swap", swapStatic);

  v8utils::ignoreMaybeResult(
      ctorObject->Set(context, v8::String::NewFromUtf8(isolate, "from", v8::NewStringType::kInternalized).ToLocalChecked(), ctorFunction));

  v8utils::defineHiddenField(isolate, ctorObject, "default", ctorFunction);
  v8utils::defineHiddenField(isolate, ctorObject, "RoaringBitmap32", ctorFunction);

  v8utils::ignoreMaybeResult(exports->Set(context, className, ctorFunction));
  constructor.Set(isolate, ctorFunction);
}

RoaringBitmap32::RoaringBitmap32() : roaring{}, version(0) {
}

RoaringBitmap32::~RoaringBitmap32() {
  this->destroy();
}

void RoaringBitmap32::destroy() {
  if (!persistent.IsEmpty()) {
    persistent.ClearWeak();
    persistent.Reset();
  }
  ra_clear(&roaring.high_low_container);
}

void RoaringBitmap32::WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32> const & info) {
  RoaringBitmap32 * p = info.GetParameter();
  delete p;
}

void RoaringBitmap32::New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = constructor.Get(isolate);
    if (info.Length() < 1) {
      v8::MaybeLocal<v8::Object> v = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
      if (!v.IsEmpty()) {
        info.GetReturnValue().Set(v.ToLocalChecked());
      }
    } else {
      v8::Local<v8::Value> argv[1] = {info[0]};
      auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
      if (!v.IsEmpty()) {
        info.GetReturnValue().Set(v.ToLocalChecked());
      }
    }
    return;
  }

  auto holder = info.Holder();

  RoaringBitmap32 * instance;
  instance = new RoaringBitmap32();
  if (!instance) {
    return v8utils::throwError(isolate, "RoaringBitmap32::ctor - failed to create native RoaringBitmap32 instance");
  }

  holder->SetAlignedPointerInInternalField(0, instance);
  instance->persistent.Reset(isolate, holder);
  instance->persistent.SetWeak(instance, WeakCallback, v8::WeakCallbackType::kParameter);

  bool hasParameter = info.Length() != 0 && !info[0]->IsUndefined() && !info[0]->IsNull();
  if (hasParameter) {
    if (RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0])) {
      instance->copyFrom(info);
    } else {
      instance->addMany(info);
    }
  }

  info.GetReturnValue().Set(holder);
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
  v8::Isolate * isolate = info.GetIsolate();

  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    info.GetReturnValue().Set(false);
  } else {
    const RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<const RoaringBitmap32>(info.Holder());
    info.GetReturnValue().Set(roaring_bitmap_contains(&self->roaring, v));
  }
}

void RoaringBitmap32::hasRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  double minimum, maximum;
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[0]->NumberValue(context).To(&minimum) ||
      !info[1]->NumberValue(context).To(&maximum)) {
    return info.GetReturnValue().Set(false);
  }

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
  v8::Isolate * isolate = info.GetIsolate();

  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(0);
  }

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set((double)roaring_bitmap_rank(&self->roaring, v));
}

void RoaringBitmap32::select(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  uint32_t v;
  if (info.Length() >= 1 && info[0]->IsUint32() && info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (roaring_bitmap_select(&self->roaring, v, &v)) {
      info.GetReturnValue().Set(v);
    }
  }
}

void RoaringBitmap32::removeRunCompression(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  bool removed = roaring_bitmap_remove_run_compression(&self->roaring);
  if (removed) {
    self->invalidate();
  }
  info.GetReturnValue().Set(removed);
}

void RoaringBitmap32::runOptimize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set(roaring_bitmap_run_optimize(&self->roaring));
}

void RoaringBitmap32::shrinkToFit(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  info.GetReturnValue().Set((double)roaring_bitmap_shrink_to_fit(&self->roaring));
  self->invalidate();
}

void RoaringBitmap32::toUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  auto size = roaring_bitmap_get_cardinality(&self->roaring);

  v8::Local<v8::Value> argv[1] = {v8::Uint32::NewFromUnsigned(isolate, (uint32_t)size)};
  auto typedArrayMaybe = JSTypes::Uint32Array_ctor.Get(isolate)->NewInstance(isolate->GetCurrentContext(), 1, argv);
  if (typedArrayMaybe.IsEmpty())
    return;

  auto typedArray = typedArrayMaybe.ToLocalChecked();

  if (size != 0) {
    const v8utils::TypedArrayContent<uint32_t> typedArrayContent(typedArray);
    if (!typedArrayContent.length || !typedArrayContent.data)
      return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - failed to allocate memory");

    roaring_bitmap_to_uint32_array(&self->roaring, typedArrayContent.data);
  }

  info.GetReturnValue().Set(typedArray);
}

void RoaringBitmap32::toArray(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto returnValue = JSTypes::Array_from.Get(isolate)->Call(isolate->GetCurrentContext(), JSTypes::Array.Get(isolate), 1, argv);
  if (returnValue.IsEmpty()) {
    return v8utils::throwError(isolate, "RoaringBitmap32::toArray - failed to generate array");
  }
  info.GetReturnValue().Set(returnValue.ToLocalChecked());
}

void RoaringBitmap32::toSet(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto v = JSTypes::Set_ctor.Get(isolate)->NewInstance(isolate->GetCurrentContext(), 1, argv);
  if (!v.IsEmpty()) {
    v8::Local<v8::Value> returnValue = v.ToLocalChecked();
    info.GetReturnValue().Set(returnValue);
  }
}

void RoaringBitmap32::toString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  std::string result("RoaringBitmap32:");
  result += std::to_string(self ? roaring_bitmap_get_cardinality(&self->roaring) : 0);
  auto returnValue = v8::String::NewFromUtf8(isolate, result.c_str(), v8::NewStringType::kNormal);
  if (returnValue.IsEmpty()) {
    info.GetReturnValue().Set(v8::String::Empty(isolate));
  } else {
    info.GetReturnValue().Set(returnValue.ToLocalChecked());
  }
}

void RoaringBitmap32::contentToString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  struct iter_data {
    std::string str;
    char first_char = '[';
    uint64_t maxLen = 32000;
  } iterData;

  double nv;
  if (info.Length() >= 1 && info[0]->IsNumber() && info[0]->NumberValue(isolate->GetCurrentContext()).To(&nv) && nv >= 0) {
    iterData.maxLen = (uint64_t)nv;
  }

  if (self && !roaring_bitmap_is_empty(&self->roaring)) {
    roaring_iterate(
        &self->roaring,
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

  auto returnValue = v8::String::NewFromUtf8(isolate, iterData.str.c_str(), v8::NewStringType::kNormal);
  if (returnValue.IsEmpty()) {
    info.GetReturnValue().Set(v8::String::Empty(isolate));
  } else {
    info.GetReturnValue().Set(returnValue.ToLocalChecked());
  }
}

void RoaringBitmap32::clone(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
  if (!v.IsEmpty()) {
    v8::Local<v8::Value> returnValue = v.ToLocalChecked();
    info.GetReturnValue().Set(returnValue);
  }
}

void RoaringBitmap32::statistics(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_statistics_t stats;
  roaring_bitmap_statistics(&self->roaring, &stats);
  auto context = isolate->GetCurrentContext();
  auto result = v8::Object::New(isolate);
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "containers", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.n_containers)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "arrayContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.n_array_containers)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "runContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.n_run_containers)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "bitsetContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.n_bitset_containers)));
  v8utils::ignoreMaybeResult(
      result->Set(context, v8::String::NewFromUtf8(isolate, "valuesInArrayContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
          v8::Uint32::NewFromUnsigned(isolate, stats.n_values_array_containers)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "valuesInRunContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.n_values_run_containers)));
  v8utils::ignoreMaybeResult(
      result->Set(context, v8::String::NewFromUtf8(isolate, "valuesInBitsetContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
          v8::Uint32::NewFromUnsigned(isolate, stats.n_values_bitset_containers)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "bytesInArrayContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_array_containers)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "bytesInRunContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_run_containers)));
  v8utils::ignoreMaybeResult(
      result->Set(context, v8::String::NewFromUtf8(isolate, "bytesInBitsetContainers", v8::NewStringType::kInternalized).ToLocalChecked(),
          v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_bitset_containers)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "maxValue", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.max_value)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "minValue", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Uint32::NewFromUnsigned(isolate, stats.min_value)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "sumOfAllValues", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Number::New(isolate, (double)stats.sum_value)));
  v8utils::ignoreMaybeResult(result->Set(context, v8::String::NewFromUtf8(isolate, "size", v8::NewStringType::kInternalized).ToLocalChecked(),
      v8::Number::New(isolate, (double)stats.cardinality)));
  info.GetReturnValue().Set(result);
}

//////////// RoaringBitmap32FactoryAsyncWorker ////////////

RoaringBitmap32FactoryAsyncWorker::RoaringBitmap32FactoryAsyncWorker(v8::Isolate * isolate) : v8utils::AsyncWorker(isolate), bitmap{}, bitmapMoved(false) {
}

RoaringBitmap32FactoryAsyncWorker::~RoaringBitmap32FactoryAsyncWorker() {
  if (!bitmapMoved) {
    ra_clear(&bitmap.high_low_container);
  }
}

v8::Local<v8::Value> RoaringBitmap32FactoryAsyncWorker::done() {
  v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty()) {
    ra_clear(&bitmap.high_low_container);
    bitmap.high_low_container = roaring_array_t{};
    return empty();
  }

  v8::Local<v8::Object> result = resultMaybe.ToLocalChecked();

  RoaringBitmap32 * unwrapped = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  unwrapped->roaring = std::move(bitmap);
  bitmapMoved = true;

  return result;
}

///// Serialization /////

void RoaringBitmap32::getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
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

void RoaringBitmap32::serialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  bool portable = info.Length() > 0 && info[0]->IsTrue();
  auto portablesize = roaring_bitmap_portable_size_in_bytes(&self->roaring);

  if (portable) {
    auto typedArray = JSTypes::bufferAllocUnsafe(isolate, portablesize);
    const v8utils::TypedArrayContent<uint8_t> buf(typedArray);
    if (!buf.length || !buf.data)
      return v8utils::throwError(isolate, "RoaringBitmap32::serialize - failed to allocate");

    roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data);
    info.GetReturnValue().Set(typedArray);
  } else {
    auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
    auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

    if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
      auto typedArray = JSTypes::bufferAllocUnsafe(isolate, portablesize + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError(isolate, "RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_CONTAINER;
      roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data + 1);
      info.GetReturnValue().Set(typedArray);
    } else {
      auto typedArray = JSTypes::bufferAllocUnsafe(isolate, (size_t)sizeasarray + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError(isolate, "RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_ARRAY_UINT32;
      memcpy(buf.data + 1, &cardinality, sizeof(uint32_t));
      roaring_bitmap_to_uint32_array(&self->roaring, (uint32_t *)(buf.data + 1 + sizeof(uint32_t)));
      info.GetReturnValue().Set(typedArray);
    }
  }
}

const char * RoaringBitmap32::doDeserialize(const v8utils::TypedArrayContent<uint8_t> & typedArray, bool portable, roaring_bitmap_t & bitmap) {
  bitmap = roaring_bitmap_t{};

  auto bufLen = typedArray.length;
  const char * bufaschar = (const char *)typedArray.data;

  if (bufLen == 0 || !bufaschar) {
    return nullptr;
  }

  if (portable) {
    size_t bytesread;
    bool ok = ra_portable_deserialize(&bitmap.high_low_container, bufaschar, bufLen, &bytesread);
    if (!ok) {
      return "RoaringBitmap32::deserialize - portable deserialization failed";
    }
    return nullptr;
  }

  switch ((unsigned char)bufaschar[0]) {
    case SERIALIZATION_ARRAY_UINT32: {
      uint32_t card;
      memcpy(&card, bufaschar + 1, sizeof(uint32_t));

      if (card * sizeof(uint32_t) + sizeof(uint32_t) + 1 != bufLen) {
        return "RoaringBitmap32::deserialize - corrupted data, wrong cardinality header";
      }

      const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
      roaring_bitmap_add_many(&bitmap, card, elems);
      roaring_bitmap_run_optimize(&bitmap);
      roaring_bitmap_shrink_to_fit(&bitmap);
      return nullptr;
    }

    case SERIALIZATION_CONTAINER: {
      size_t bytesread;
      bool ok = ra_portable_deserialize(&bitmap.high_low_container, bufaschar + 1, bufLen - 1, &bytesread);
      if (!ok) {
        return "RoaringBitmap32::deserialize - deserialization failed";
      }
      return nullptr;
    }
  }

  return "RoaringBitmap32::deserialize - invalid portable header byte";
}

void RoaringBitmap32::deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer");

  const v8utils::TypedArrayContent<uint8_t> typedArray(info[0]);

  roaring_bitmap_t bitmap;
  const char * error = doDeserialize(typedArray, info.Length() > 1 && info[1]->IsTrue(), bitmap);
  if (error != nullptr) {
    return v8utils::throwError(isolate, error);
  }

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty()) {
    ra_clear(&bitmap.high_low_container);
    bitmap.high_low_container = roaring_array_t{};
    return;
  }

  v8::Local<v8::Object> result = resultMaybe.ToLocalChecked();

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  self->roaring = std::move(bitmap);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::deserialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  auto isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer");

  auto holder = info.Holder();

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(holder);

  const v8utils::TypedArrayContent<uint8_t> typedArray(info[0]);
  roaring_bitmap_t bitmap;
  const char * error = doDeserialize(typedArray, info.Length() > 1 && info[1]->IsTrue(), bitmap);
  if (error != nullptr) {
    return v8utils::throwError(isolate, error);
  }

  self->invalidate();
  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(bitmap.high_low_container);

  info.GetReturnValue().Set(holder);
}

class DeserializeWorker : public RoaringBitmap32FactoryAsyncWorker {
 public:
  v8::Persistent<v8::Value> bufferPersistent;
  v8utils::TypedArrayContent<uint8_t> buffer;
  bool portable;

  DeserializeWorker(v8::Isolate * isolate) : RoaringBitmap32FactoryAsyncWorker(isolate), portable(false) {
  }

  virtual ~DeserializeWorker() {
    bufferPersistent.Reset();
  }

  bool setBuffer(v8::Local<v8::Value> buffer) {
    if (buffer.IsEmpty() || (!buffer->IsUint8Array() && !buffer->IsInt8Array() && !buffer->IsUint8ClampedArray())) {
      return false;
    }
    bufferPersistent.Reset(isolate, buffer);
    if (!this->buffer.set(buffer)) {
      return false;
    }
    return true;
  }

 protected:
  virtual void work() {
    this->setError(RoaringBitmap32::doDeserialize(buffer, portable, bitmap));
  }
};

void RoaringBitmap32::deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  DeserializeWorker * worker = new DeserializeWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32::deserializeAsync - Failed to allocate async worker");
  }

  if (info.Length() <= 0 || !worker->setBuffer(info[0])) {
    delete worker;
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::deserializeAsync - first argument must be Uint8Array or Buffer");
  }

  if (info.Length() >= 2) {
    if (info[1]->IsFunction()) {
      worker->setCallback(info[1]);
      if (info.Length() >= 3 && info[2]->IsTrue()) {
        worker->portable = true;
      }
    } else {
      if (info[1]->IsTrue()) {
        worker->portable = true;
      }
      if (info.Length() >= 3 && info[2]->IsFunction()) {
        worker->setCallback(info[2]);
      }
    }
  }

  v8::Local<v8::Value> returnValue = v8utils::AsyncWorker::run(worker);
  info.GetReturnValue().Set(returnValue);
}

class DeserializeParallelWorkerItem {
 public:
  roaring_bitmap_t bitmap;
  v8::Persistent<v8::Value> bufferPersistent;
  v8utils::TypedArrayContent<uint8_t> buffer;
  bool moved;

  DeserializeParallelWorkerItem() : bitmap{}, moved(false) {
  }

  ~DeserializeParallelWorkerItem() {
    bufferPersistent.Reset();
    if (!moved) {
      ra_clear(&bitmap.high_low_container);
    }
  }

  bool setBuffer(v8::Isolate * isolate, v8::Local<v8::Value> buffer) {
    if (buffer.IsEmpty() || (!buffer->IsUint8Array() && !buffer->IsInt8Array() && !buffer->IsUint8ClampedArray())) {
      return false;
    }
    if (!this->buffer.set(buffer)) {
      return false;
    }
    return true;
  }
};

class DeserializeParallelWorker : public v8utils::ParallelAsyncWorker {
 public:
  v8::Persistent<v8::Value> bufferPersistent;

  bool portable;
  DeserializeParallelWorkerItem * items;

  DeserializeParallelWorker(v8::Isolate * isolate) : v8utils::ParallelAsyncWorker(isolate), portable(false), items(nullptr) {
  }

  virtual ~DeserializeParallelWorker() {
    if (items != nullptr) {
      delete[] items;
    }
  }

 protected:
  virtual void parallelWork(uint32_t index) {
    DeserializeParallelWorkerItem & item = items[index];
    const char * error = RoaringBitmap32::doDeserialize(item.buffer, portable, item.bitmap);
    if (error != nullptr) {
      this->setError(error);
    }
  }

  virtual v8::Local<v8::Value> done() {
    v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

    const uint32_t itemsCount = this->loopCount;
    DeserializeParallelWorkerItem * items = this->items;

    v8::MaybeLocal<v8::Array> resultArrayMaybe = v8::Array::New(isolate, itemsCount);
    if (resultArrayMaybe.IsEmpty()) {
      return empty();
    }

    v8::Local<v8::Array> resultArray = resultArrayMaybe.ToLocalChecked();
    v8::Local<v8::Context> currentContext = isolate->GetCurrentContext();

    for (uint32_t i = 0; i != itemsCount; ++i) {
      v8::MaybeLocal<v8::Object> instanceMaybe = cons->NewInstance(currentContext, 0, nullptr);
      if (instanceMaybe.IsEmpty()) {
        return empty();
      }

      v8::Local<v8::Object> instance = instanceMaybe.ToLocalChecked();

      RoaringBitmap32 * unwrapped = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(instance);

      DeserializeParallelWorkerItem & item = items[i];

      unwrapped->roaring = std::move(item.bitmap);
      item.moved = true;

      v8utils::ignoreMaybeResult(resultArray->Set(currentContext, i, instance));
    }

    return resultArray;
  }
};

void RoaringBitmap32::deserializeParallelStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !info[0]->IsArray()) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::deserializeParallelAsync requires an array as first argument");
  }

  auto array = v8::Local<v8::Array>::Cast(info[0]);

  uint32_t length = array->Length();

  if (length > 0x01FFFFFF) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::deserializeParallelAsync - array too big");
  }

  DeserializeParallelWorker * worker = new DeserializeParallelWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "Failed to allocate async worker");
  }

  DeserializeParallelWorkerItem * items = length ? new DeserializeParallelWorkerItem[length]() : nullptr;
  if (items == nullptr && length != 0) {
    delete worker;
    return v8utils::throwError(isolate, "Failed to allocate async worker memory");
  }
  worker->items = items;
  worker->loopCount = length;

  auto context = isolate->GetCurrentContext();
  for (uint32_t i = 0; i != length; ++i) {
    auto vMaybe = array->Get(context, i);
    v8::Local<v8::Value> v;
    if (!vMaybe.ToLocal(&v) || (!v->IsNullOrUndefined() && !items[i].setBuffer(isolate, v))) {
      delete worker;
      return v8utils::throwTypeError(isolate, "RoaringBitmap32::deserializeParallelAsync - Array must contains only Uint8Array or Buffer elements");
    }
  }

  if (info.Length() >= 2) {
    if (info[1]->IsFunction()) {
      worker->setCallback(info[1]);
      if (info.Length() >= 3 && info[2]->IsTrue()) {
        worker->portable = true;
      }
    } else {
      if (info[1]->IsTrue()) {
        worker->portable = true;
      }
      if (info.Length() >= 3 && info[2]->IsFunction()) {
        worker->setCallback(info[2]);
      }
    }
  }

  v8::Local<v8::Value> returnValue = v8utils::AsyncWorker::run(worker);
  info.GetReturnValue().Set(returnValue);
}

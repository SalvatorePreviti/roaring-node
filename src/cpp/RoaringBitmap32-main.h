#include "includes.h"

/////////////////// unity build ///////////////////

#include "RoaringBitmap32.h"
#include "RoaringBitmap32-ops.h"
#include "RoaringBitmap32-static-ops.h"
#include "RoaringBitmap32-serialization.h"
#include "RoaringBitmap32-ranges.h"

void RoaringBitmap32_copyFrom(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (info.Length() == 0 || !roaringAddMany(isolate, self, info[0], true)) {
    return v8utils::throwError(
      isolate, "RoaringBitmap32::copyFrom expects a RoaringBitmap32, an Uint32Array or an Iterable");
  }
  self->invalidate();
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32_WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32> const & info) {
  RoaringBitmap32 * p = info.GetParameter();
  if (p != nullptr) {
    p->~RoaringBitmap32();
    bare_aligned_free(p);
  }
}

void RoaringBitmap32_New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  AddonData * addonData = AddonData::get(info);
  if (addonData == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);
    v8::MaybeLocal<v8::Object> v;
    if (info.Length() < 1) {
      v = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    } else {
      v8::Local<v8::Value> argv[1] = {info[0]};
      v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
    }

    v8::Local<v8::Object> vlocal;
    if (v.ToLocal(&vlocal)) {
      info.GetReturnValue().Set(vlocal);
    }
    return;
  }

  auto holder = info.Holder();

  // If first parameter is a RoaringBitmap32 instance,
  // and the second parameter is "readonly", creates a readonly bitmap.

  RoaringBitmap32 * readonlyViewOf = nullptr;

  if (info.Length() >= 2 && info[1]->IsString()) {
    v8::String::Utf8Value arg1String(isolate, info[1]);
    if (strcmp(*arg1String, "readonly") == 0) {
      readonlyViewOf = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
      if (readonlyViewOf == nullptr) {
        return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
      }
    }
  }

  auto * instanceMemory = bare_aligned_malloc(32, sizeof(RoaringBitmap32));
  RoaringBitmap32 * instance;
  bool hasCapacity = false;

  if (readonlyViewOf) {
    instance = new (instanceMemory) RoaringBitmap32(addonData, readonlyViewOf);
  } else {
    if (info.Length() > 0 && info[0]->IsNumber()) {
      hasCapacity = true;
      double capacity;
      if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&capacity)) {
        capacity = 0;
      }
      if (capacity < 0) {
        capacity = 0;
      } else if (capacity > UINT32_MAX) {
        capacity = UINT32_MAX;
      }
      instance = new (instanceMemory) RoaringBitmap32(addonData, (uint32_t)capacity);
    } else {
      instance = new (instanceMemory) RoaringBitmap32(addonData, 0U);
    }
  }

  if (instance == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32::ctor - failed to create RoaringBitmap32 instance");
  }

  if (instance->roaring == nullptr) {
    instance->~RoaringBitmap32();
    bare_aligned_free(instance);
    return v8utils::throwError(isolate, "RoaringBitmap32::ctor - failed to create native RoaringBitmap32 instance");
  }

  int indices[2] = {0, 1};
  void * values[2] = {instance, (void *)(RoaringBitmap32::OBJECT_TOKEN)};
  holder->SetAlignedPointerInInternalFields(2, indices, values);

  instance->persistent.Reset(isolate, holder);
  instance->persistent.SetWeak(instance, RoaringBitmap32_WeakCallback, v8::WeakCallbackType::kParameter);

  if (readonlyViewOf) {
    instance->frozenStorage.bufferPersistent.Reset(isolate, holder);
  } else if (!hasCapacity) {
    bool hasParameter = info.Length() != 0 && !info[0]->IsUndefined() && !info[0]->IsNull();
    if (hasParameter) {
      if (addonData->RoaringBitmap32_constructorTemplate.Get(isolate)->HasInstance(info[0])) {
        RoaringBitmap32_copyFrom(info);
      } else {
        RoaringBitmap32_addMany(info);
      }
    }
  }

  info.GetReturnValue().Set(holder);
}

void RoaringBitmap32_ofStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  auto context = isolate->GetCurrentContext();

  int len = info.Length();
  if (len < 0) {
    len = 0;
  }

  AddonData * addonData = AddonData::get(info);
  if (addonData == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  auto newRoaringBitmap32ObjectMaybe = addonData->RoaringBitmap32_constructor.Get(isolate)->NewInstance(context, 0, nullptr);

  v8::Local<v8::Object> newRoaringBitmap32Object;
  if (!newRoaringBitmap32ObjectMaybe.ToLocal(&newRoaringBitmap32Object)) {
    return;
  }

  info.GetReturnValue().Set(newRoaringBitmap32Object);

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(newRoaringBitmap32Object, isolate);
  if (self == nullptr || !self->roaring) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  auto roaring = self->roaring;
  uint32_t v = 0;
  for (int i = 0; i < len; ++i) {
    auto arg = info[i];
    if (arg->IsUint32()) {
      if (!arg->Uint32Value(context).To(&v)) {
        continue;
      }
    } else if (arg->IsInt32()) {
      int32_t n;
      if (!arg->Int32Value(context).To(&n) || n < 0) {
        continue;
      }
      v = (uint32_t)n;
    } else {
      double d;
      if (arg->IsNull() || arg->IsUndefined()) {
        continue;
      }
      if (!arg->NumberValue(context).To(&d) || std::isnan(d)) {
        continue;
      }
      int64_t n = (int64_t)d;
      if (n < 0 || n > UINT32_MAX) {
        continue;
      }
      v = (uint32_t)n;
    }
    roaring_bitmap_add(roaring, v);
  }
  self->invalidate();
}

void RoaringBitmap32_getInstanceCountStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  AddonData * addonData = AddonData::get(info);
  info.GetReturnValue().Set(addonData ? (double)(addonData->RoaringBitmap32_instances) : 0.0);
}

void RoaringBitmap32_asReadonlyView(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  v8::Local<v8::Object> vlocal;
  if (!self->readonlyViewPersistent.IsEmpty()) {
    vlocal = self->readonlyViewPersistent.Get(isolate);
    if (vlocal->IsObject()) {
      info.GetReturnValue().Set(vlocal);
      return;
    }
  }

  if (self->isFrozenForever()) {
    self->readonlyViewPersistent.Reset(isolate, info.Holder());
    info.GetReturnValue().Set(info.Holder());
    return;
  }

  AddonData * addonData = self->addonData;
  v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);
  v8::MaybeLocal<v8::Object> v;
  v8::Local<v8::Value> argv[2] = {info.Holder(), addonData->strings.readonly.Get(isolate)};
  v = cons->NewInstance(isolate->GetCurrentContext(), 2, argv);

  if (!v.ToLocal(&vlocal)) {
    return v8utils::throwError(
      isolate, "RoaringBitmap32::asReadonlyView - failed to create readonly RoaringBitmap32 instance");
  }

  self->readonlyViewPersistent.Reset(isolate, vlocal);

  info.GetReturnValue().Set(vlocal);
}

void RoaringBitmap32_size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  auto size = self != nullptr ? self->getSize() : 0U;
  return size <= 0xFFFFFFFF ? info.GetReturnValue().Set((uint32_t)size) : info.GetReturnValue().Set((double)size);
}

void RoaringBitmap32_isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  auto result = self == nullptr || self->isEmpty();
  return info.GetReturnValue().Set(result);
}

void RoaringBitmap32_isFrozen_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  info.GetReturnValue().Set(self == nullptr || self->isFrozen());
}

void RoaringBitmap32_has(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), isolate);
  uint32_t v;
  if (
    self == nullptr || info.Length() < 1 || !info[0]->IsUint32() ||
    !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    info.GetReturnValue().Set(false);
  } else {
    info.GetReturnValue().Set(roaring_bitmap_contains(self->roaring, v));
  }
}

void RoaringBitmap32_indexOf(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), isolate);
  uint32_t v;
  if (
    self == nullptr || info.Length() < 1 || !info[0]->IsUint32() ||
    !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(-1);
  }
  int64_t r = roaring_bitmap_get_index(self->roaring, v);
  if (r >= 0) {
    int64_t fromIndex;
    if (info.Length() > 1 && info[1]->IntegerValue(isolate->GetCurrentContext()).To(&fromIndex)) {
      if (fromIndex < 0) {
        fromIndex += self->getSize();
      }
      if (r < fromIndex) {
        return info.GetReturnValue().Set(-1);
      }
    }
  }
  return (uint64_t)r <= 0xFFFFFFFFull
    ? r < 0x7fffffff ? info.GetReturnValue().Set((int)r) : info.GetReturnValue().Set((uint32_t)r)
    : info.GetReturnValue().Set((double)r);
}

void RoaringBitmap32_lastIndexOf(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), isolate);
  uint32_t v;
  if (
    self == nullptr || info.Length() < 1 || !info[0]->IsUint32() ||
    !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(-1);
  }
  int64_t r = roaring_bitmap_get_index(self->roaring, v);
  if (r >= 0) {
    int64_t fromIndex;
    if (info.Length() > 1 && info[1]->IntegerValue(isolate->GetCurrentContext()).To(&fromIndex)) {
      if (fromIndex < 0) {
        fromIndex += self->getSize();
      }
      if (r > fromIndex) {
        return info.GetReturnValue().Set(-1);
      }
    }
  }
  return (uint64_t)r <= 0xFFFFFFFFull
    ? r < 0x7fffffff ? info.GetReturnValue().Set((int)r) : info.GetReturnValue().Set((uint32_t)r)
    : info.GetReturnValue().Set((double)r);
}

void RoaringBitmap32_minimum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  return info.GetReturnValue().Set(self != nullptr ? roaring_bitmap_minimum(self->roaring) : 0);
}

void RoaringBitmap32_maximum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  return info.GetReturnValue().Set(self != nullptr ? roaring_bitmap_maximum(self->roaring) : 0);
}

void RoaringBitmap32_rank(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return info.GetReturnValue().Set(0);
  }
  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(0);
  }
  info.GetReturnValue().Set((double)roaring_bitmap_rank(self->roaring, v));
}

void RoaringBitmap32_select(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return info.GetReturnValue().Set(0);
  }
  uint32_t v;
  if (info.Length() >= 1 && info[0]->IsUint32() && info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    if (roaring_bitmap_select(self->roaring, v, &v)) {
      info.GetReturnValue().Set(v);
    }
  }
}

void RoaringBitmap32_removeRunCompression(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }
  if (self->isFrozenHard()) {
    return v8utils::throwError(info.GetIsolate(), ERROR_FROZEN);
  }
  bool removed = roaring_bitmap_remove_run_compression(self->roaring);
  if (removed) {
    self->invalidate();
  }
  info.GetReturnValue().Set(removed);
}

void RoaringBitmap32_runOptimize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozenHard()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  info.GetReturnValue().Set(roaring_bitmap_run_optimize(self->roaring));
}

void RoaringBitmap32_shrinkToFit(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozenHard()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  info.GetReturnValue().Set((double)roaring_bitmap_shrink_to_fit(self->roaring));
}

void RoaringBitmap32_freeze(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self && self->frozenCounter >= 0) {
    self->frozenCounter = RoaringBitmap32::FROZEN_COUNTER_SOFT_FROZEN;
  }
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32_clone(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  v8::Local<v8::Function> cons = self->addonData->RoaringBitmap32_constructor.Get(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
  v8::Local<v8::Object> vlocal;
  if (v.ToLocal(&vlocal)) {
    info.GetReturnValue().Set(vlocal);
  }
}

void RoaringBitmap32_statistics(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  roaring_statistics_t stats;
  roaring_bitmap_statistics(self->roaring, &stats);
  auto context = isolate->GetCurrentContext();
  auto result = v8::Object::New(isolate);
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "containers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "arrayContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_array_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "runContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_run_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bitsetContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bitset_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "valuesInArrayContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_values_array_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "valuesInRunContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_values_run_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "valuesInBitsetContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_values_bitset_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bytesInArrayContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_array_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bytesInRunContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_run_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bytesInBitsetContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_bitset_containers)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "maxValue", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.max_value)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "minValue", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.min_value)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "sumOfAllValues", v8::NewStringType::kInternalized),
    v8::Number::New(isolate, (double)stats.sum_value)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "size", v8::NewStringType::kInternalized),
    v8::Number::New(isolate, (double)stats.cardinality)));
  ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "isFrozen", v8::NewStringType::kInternalized),
    v8::Boolean::New(isolate, self->isFrozen())));
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32_isSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self == other || (self && other && roaring_bitmap_is_subset(self->roaring, other->roaring)));
}

void RoaringBitmap32_isStrictSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self && other && roaring_bitmap_is_strict_subset(self->roaring, other->roaring));
}

void RoaringBitmap32_isEqual(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(
    self == other || (self && other && roaring_bitmap_equals(self->roaring, other->roaring) ? true : false));
}

void RoaringBitmap32_intersects(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self && other && roaring_bitmap_intersect(self->roaring, other->roaring) ? true : false);
}

void RoaringBitmap32_jaccardIndex(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self && other ? roaring_bitmap_jaccard_index(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32_swapStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap expects 2 arguments");

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap second argument must be a RoaringBitmap32");

  if (a->isFrozen() || b->isFrozen()) return v8utils::throwError(isolate, ERROR_FROZEN);

  if (a != b) {
    auto * a_roaring = a->roaring;
    auto a_sizeCache = a->sizeCache;
    a->roaring = b->roaring;
    a->sizeCache = b->sizeCache;
    b->roaring = a_roaring;
    b->sizeCache = a_sizeCache;

    a->invalidate();
    b->invalidate();
  }
}

void RoaringBitmap32_toString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  info.GetReturnValue().Set(self->addonData->strings.RoaringBitmap32.Get(isolate));
}

void RoaringBitmap32_at(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (info.Length() < 1) {
    return;
  }
  uint32_t index;
  if (info[0]->IsInt32()) {
    int64_t n = info[0]->Int32Value(isolate->GetCurrentContext()).FromJust();
    if (n < 0) {
      n += (int64_t)self->getSize();
      if (n < 0 || n > UINT32_MAX) {
        return;
      }
    }
    index = (uint32_t)n;
  } else if (info[0]->IsUint32()) {
    index = info[0]->Uint32Value(isolate->GetCurrentContext()).FromJust();
  } else {
    if (info[0]->IsNull() || info[0]->IsUndefined()) {
      return;
    }
    double d;
    if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&d) || std::isnan(d)) {
      return;
    }
    int64_t n = (int64_t)d;
    if (n < 0) {
      n += (int64_t)self->getSize();
      if (n < 0) {
        return;
      }
    }
    if (n > UINT32_MAX) {
      return;
    }
    index = (uint32_t)n;
  }

  uint32_t result;
  if (roaring_bitmap_select(self->roaring, index, &result)) {
    info.GetReturnValue().Set(result);
  }
}

void RoaringBitmap32_join(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  struct iter_data {
    uint32_t count;
    std::string sep;
    std::string result;
  } iterData;

  iterData.count = 0;

  if (info.Length() >= 1 && info[0]->IsString()) {
    v8::String::Utf8Value sep(isolate, info[0]);
    iterData.sep = *sep;
  } else {
    iterData.sep = ",";
  }

  if (self != nullptr && !self->isEmpty()) {
    roaring_iterate(
      self->roaring,
      [](uint32_t value, void * vp) -> bool {
        auto * p = reinterpret_cast<iter_data *>(vp);
        if (p->result.length() >= 536870880) {
          return false;
        }

        if (p->count++ > 0 && !p->sep.empty()) {
          p->result.append(p->sep);
        }
        p->result.append(std::to_string(value));
        return true;
      },
      (void *)&iterData);
  }

  auto returnValue = v8::String::NewFromUtf8(isolate, iterData.result.c_str(), v8::NewStringType::kNormal);
  v8::Local<v8::String> returnValueLocal;
  if (returnValue.ToLocal(&returnValueLocal)) {
    info.GetReturnValue().Set(returnValueLocal);
  } else {
    info.GetReturnValue().Set(v8::String::Empty(isolate));
  }
}

void RoaringBitmap32_contentToString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  struct iter_data {
    std::string str;
    char first_char = '[';
    uint64_t maxLen = 32000;
  } iterData;

  double nv;
  if (info.Length() >= 1 && info[0]->IsNumber() && info[0]->NumberValue(isolate->GetCurrentContext()).To(&nv) && nv >= 0) {
    iterData.maxLen = (uint64_t)nv;
  }

  if (iterData.maxLen > 536870880) {
    iterData.maxLen = 536870880;
  }

  if (self != nullptr && !self->isEmpty()) {
    roaring_iterate(
      self->roaring,
      [](uint32_t value, void * vp) -> bool {
        auto * p = reinterpret_cast<iter_data *>(vp);
        if (p->str.length() >= p->maxLen) {
          p->str.append("...");
          return false;
        }

        p->str += p->first_char;
        p->str.append(std::to_string(value));
        p->first_char = ',';
        return true;
      },
      (void *)&iterData);
  } else {
    iterData.str = '[';
  }
  iterData.str += ']';

  auto returnValue = v8::String::NewFromUtf8(isolate, iterData.str.c_str(), v8::NewStringType::kNormal);
  v8::Local<v8::String> returnValueLocal;
  if (returnValue.ToLocal(&returnValueLocal)) {
    info.GetReturnValue().Set(returnValueLocal);
  } else {
    info.GetReturnValue().Set(v8::String::Empty(isolate));
  }
}

void RoaringBitmap32_fromArrayStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  v8::Local<v8::Value> arg;

  if (info.Length() >= 1) {
    arg = info[0];
  }

  AddonData * addonData = AddonData::get(info);
  if (addonData == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  auto * worker = new FromArrayAsyncWorker(isolate, addonData);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "Failed to allocate async worker");
  }

  if (!arg->IsNullOrUndefined() && !arg->IsFunction()) {
    if (arg->IsUint32Array() || arg->IsInt32Array()) {
      worker->buffer.set(isolate, arg);
      const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, arg);
    } else if (arg->IsObject()) {
      if (addonData->RoaringBitmap32_constructorTemplate.Get(isolate)->HasInstance(arg)) {
        return v8utils::throwTypeError(
          isolate, "RoaringBitmap32::fromArrayAsync cannot be called with a RoaringBitmap32 instance");
      }
      v8::Local<v8::Value> argv[] = {arg};
      auto carg = addonData->Uint32Array_from.Get(isolate)->Call(
        isolate->GetCurrentContext(), addonData->Uint32Array.Get(isolate), 1, argv);
      v8::Local<v8::Value> local;
      if (carg.ToLocal(&local)) {
        worker->buffer.set(isolate, local);
      }
    }

    worker->argPersistent.Reset(isolate, arg);
  }

  if (info.Length() >= 2 && info[1]->IsFunction()) {
    worker->setCallback(info[1]);
  } else if (info.Length() >= 1 && info[0]->IsFunction()) {
    worker->setCallback(info[0]);
  }

  v8::Local<v8::Value> returnValue = AsyncWorker::run(worker);
  info.GetReturnValue().Set(returnValue);
}

void RoaringBitmap32_Init(v8::Local<v8::Object> exports, AddonData * addonData) {
  croaringMemoryInitialize();

  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::String> className = addonData->strings.RoaringBitmap32.Get(isolate);

  auto versionString = NEW_LITERAL_V8_STRING(isolate, ROARING_VERSION, v8::NewStringType::kInternalized);

  v8::Local<v8::FunctionTemplate> ctor =
    v8::FunctionTemplate::New(isolate, RoaringBitmap32_New, addonData->external.Get(isolate));
  if (ctor.IsEmpty()) {
    return;
  }
  addonData->RoaringBitmap32_constructorTemplate.Set(isolate, ctor);

  v8::Local<v8::ObjectTemplate> ctorInstanceTemplate = ctor->InstanceTemplate();
  ctor->SetClassName(className);

  ctorInstanceTemplate->SetInternalFieldCount(2);

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "isEmpty", v8::NewStringType::kInternalized),
    RoaringBitmap32_isEmpty_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "size", v8::NewStringType::kInternalized),
    RoaringBitmap32_size_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "isFrozen", v8::NewStringType::kInternalized),
    RoaringBitmap32_isFrozen_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  NODE_SET_PROTOTYPE_METHOD(ctor, "add", RoaringBitmap32_add);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addMany", RoaringBitmap32_addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addRange", RoaringBitmap32_addRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andCardinality", RoaringBitmap32_andCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andInPlace", RoaringBitmap32_andInPlace);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotCardinality", RoaringBitmap32_andNotCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotInPlace", RoaringBitmap32_removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "asReadonlyView", RoaringBitmap32_asReadonlyView);
  NODE_SET_PROTOTYPE_METHOD(ctor, "at", RoaringBitmap32_at);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clear", RoaringBitmap32_clear);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clone", RoaringBitmap32_clone);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contains", RoaringBitmap32_has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "containsRange", RoaringBitmap32_hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contentToString", RoaringBitmap32_contentToString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "copyFrom", RoaringBitmap32_copyFrom);
  NODE_SET_PROTOTYPE_METHOD(ctor, "delete", RoaringBitmap32_remove);
  NODE_SET_PROTOTYPE_METHOD(ctor, "deserialize", RoaringBitmap32_deserialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "flipRange", RoaringBitmap32_flipRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "freeze", RoaringBitmap32_freeze);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSerializationSizeInBytes", RoaringBitmap32_getSerializationSizeInBytes);
  NODE_SET_PROTOTYPE_METHOD(ctor, "has", RoaringBitmap32_has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "hasRange", RoaringBitmap32_hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "includes", RoaringBitmap32_has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "indexOf", RoaringBitmap32_indexOf);
  NODE_SET_PROTOTYPE_METHOD(ctor, "intersects", RoaringBitmap32_intersects);
  NODE_SET_PROTOTYPE_METHOD(ctor, "intersectsWithRange", RoaringBitmap32_intersectsWithRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isEqual", RoaringBitmap32_isEqual);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isStrictSubset", RoaringBitmap32_isStrictSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isSubset", RoaringBitmap32_isSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "jaccardIndex", RoaringBitmap32_jaccardIndex);
  NODE_SET_PROTOTYPE_METHOD(ctor, "join", RoaringBitmap32_join);
  NODE_SET_PROTOTYPE_METHOD(ctor, "lastIndexOf", RoaringBitmap32_lastIndexOf);
  NODE_SET_PROTOTYPE_METHOD(ctor, "maximum", RoaringBitmap32_maximum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "minimum", RoaringBitmap32_minimum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orCardinality", RoaringBitmap32_orCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orInPlace", RoaringBitmap32_addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "pop", RoaringBitmap32_pop);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rangeCardinality", RoaringBitmap32_rangeCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rangeUint32Array", RoaringBitmap32_rangeUint32Array);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rank", RoaringBitmap32_rank);
  NODE_SET_PROTOTYPE_METHOD(ctor, "remove", RoaringBitmap32_remove);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeMany", RoaringBitmap32_removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRange", RoaringBitmap32_removeRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRunCompression", RoaringBitmap32_removeRunCompression);
  NODE_SET_PROTOTYPE_METHOD(ctor, "runOptimize", RoaringBitmap32_runOptimize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "select", RoaringBitmap32_select);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serialize", RoaringBitmap32_serialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serializeAsync", RoaringBitmap32_serializeAsync);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serializeFileAsync", RoaringBitmap32_serializeFileAsync);
  NODE_SET_PROTOTYPE_METHOD(ctor, "shift", RoaringBitmap32_shift);
  NODE_SET_PROTOTYPE_METHOD(ctor, "shrinkToFit", RoaringBitmap32_shrinkToFit);
  NODE_SET_PROTOTYPE_METHOD(ctor, "statistics", RoaringBitmap32_statistics);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toArray", RoaringBitmap32_toArray);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toSet", RoaringBitmap32_toSet);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toString", RoaringBitmap32_toString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toUint32Array", RoaringBitmap32_toUint32Array);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toUint32ArrayAsync", RoaringBitmap32_toUint32ArrayAsync);
  NODE_SET_PROTOTYPE_METHOD(ctor, "tryAdd", RoaringBitmap32_tryAdd);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorCardinality", RoaringBitmap32_xorCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorInPlace", RoaringBitmap32_xorInPlace);

  auto ctorFunction = ctor->GetFunction(context).ToLocalChecked();
  auto ctorObject = ctorFunction->ToObject(context).ToLocalChecked();

  ctor->PrototypeTemplate()->Set(v8::Symbol::GetToStringTag(isolate), className);
  ctor->PrototypeTemplate()->Set(isolate, "CRoaringVersion", versionString);

  AddonData_setMethod(ctorObject, "addOffset", RoaringBitmap32_addOffsetStatic, addonData);
  AddonData_setMethod(ctorObject, "and", RoaringBitmap32_andStatic, addonData);
  AddonData_setMethod(ctorObject, "andNot", RoaringBitmap32_andNotStatic, addonData);

  v8utils::defineHiddenField(isolate, ctorObject, "default", ctorFunction);

  AddonData_setMethod(ctorObject, "deserialize", RoaringBitmap32_deserializeStatic, addonData);
  AddonData_setMethod(ctorObject, "deserializeAsync", RoaringBitmap32_deserializeAsyncStatic, addonData);
  AddonData_setMethod(ctorObject, "deserializeFileAsync", RoaringBitmap32_deserializeFileAsyncStatic, addonData);
  AddonData_setMethod(ctorObject, "deserializeParallelAsync", RoaringBitmap32_deserializeParallelStaticAsync, addonData);

  ignoreMaybeResult(
    ctorObject->Set(context, NEW_LITERAL_V8_STRING(isolate, "from", v8::NewStringType::kInternalized), ctorFunction));

  AddonData_setMethod(ctorObject, "fromArrayAsync", RoaringBitmap32_fromArrayStaticAsync, addonData);
  AddonData_setMethod(ctorObject, "fromRange", RoaringBitmap32_fromRangeStatic, addonData);
  AddonData_setMethod(ctorObject, "getInstancesCount", RoaringBitmap32_getInstanceCountStatic, addonData);
  AddonData_setMethod(ctorObject, "of", RoaringBitmap32_ofStatic, addonData);
  AddonData_setMethod(ctorObject, "or", RoaringBitmap32_orStatic, addonData);
  AddonData_setMethod(ctorObject, "orMany", RoaringBitmap32_orManyStatic, addonData);
  AddonData_setMethod(ctorObject, "swap", RoaringBitmap32_swapStatic, addonData);
  AddonData_setMethod(ctorObject, "unsafeFrozenView", RoaringBitmap32_unsafeFrozenViewStatic, addonData);
  AddonData_setMethod(ctorObject, "xor", RoaringBitmap32_xorStatic, addonData);
  AddonData_setMethod(ctorObject, "xorMany", RoaringBitmap32_xorManyStatic, addonData);

  v8utils::defineReadonlyField(isolate, ctorObject, "CRoaringVersion", versionString);
  v8utils::defineReadonlyField(isolate, exports, "CRoaringVersion", versionString);

  v8utils::defineHiddenField(isolate, ctorObject, className, ctorFunction);

  ignoreMaybeResult(exports->Set(context, className, ctorFunction));

  addonData->RoaringBitmap32_constructor.Set(isolate, ctorFunction);
}

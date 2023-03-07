#include "includes.h"

/////////////////// unity build ///////////////////

#include "RoaringBitmap32.h"
#include "RoaringBitmap32-ops.h"
#include "RoaringBitmap32-static-ops.h"
#include "RoaringBitmap32-serialization.h"
#include "RoaringBitmap32-ranges.h"

static void RoaringBitmap32_New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);
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
#if NODE_MAJOR_VERSION > 8
    v8::String::Utf8Value arg1String(isolate, info[1]);
#else
    v8::String::Utf8Value arg1String(info[1]);
#endif
    if (strcmp(*arg1String, "readonly") == 0) {
      readonlyViewOf = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
      if (readonlyViewOf == nullptr) {
        return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
      }
    }
  }

  RoaringBitmap32 * instance =
    readonlyViewOf ? new RoaringBitmap32(&globalAddonData, readonlyViewOf) : new RoaringBitmap32(&globalAddonData, 0U);

  if (instance == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32::ctor - failed to create RoaringBitmap32 instance");
  }

  if (instance->roaring == nullptr) {
    delete instance;
    return v8utils::throwError(isolate, "RoaringBitmap32::ctor - failed to create native RoaringBitmap32 instance");
  }

  holder->SetAlignedPointerInInternalField(0, instance);
  instance->persistent.Reset(isolate, holder);
  instance->persistent.SetWeak(instance, RoaringBitmap32_WeakCallback, v8::WeakCallbackType::kParameter);

  if (readonlyViewOf) {
    instance->frozenStorage.bufferPersistent.Reset(isolate, holder);
  } else {
    bool hasParameter = info.Length() != 0 && !info[0]->IsUndefined() && !info[0]->IsNull();
    if (hasParameter) {
      if (globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate)->HasInstance(info[0])) {
        RoaringBitmap32::copyFrom(info);
      } else {
        RoaringBitmap32::addMany(info);
      }
    }
  }

  info.GetReturnValue().Set(holder);
}

void RoaringBitmap32::getInstanceCountStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  info.GetReturnValue().Set((double)globalAddonData.RoaringBitmap32_instances);
}

void RoaringBitmap32::asReadonlyView(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

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

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);
  v8::MaybeLocal<v8::Object> v;
  v8::Local<v8::Value> argv[2] = {info.Holder(), globalAddonData.strings.readonly.Get(isolate)};
  v = cons->NewInstance(isolate->GetCurrentContext(), 2, argv);

  if (!v.ToLocal(&vlocal)) {
    return v8utils::throwError(
      isolate, "RoaringBitmap32::asReadonlyView - failed to create readonly RoaringBitmap32 instance");
  }

  self->readonlyViewPersistent.Reset(isolate, vlocal);

  info.GetReturnValue().Set(vlocal);
}

void RoaringBitmap32::size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  auto size = self != nullptr ? self->getSize() : 0U;
  return size <= 0xFFFFFFFF ? info.GetReturnValue().Set((uint32_t)size) : info.GetReturnValue().Set((double)size);
}

void RoaringBitmap32::isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  auto result = self == nullptr || self->isEmpty();
  return info.GetReturnValue().Set(result);
}

void RoaringBitmap32::isFrozen_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  info.GetReturnValue().Set(self == nullptr || self->isFrozen());
}

void RoaringBitmap32::has(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::minimum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  return info.GetReturnValue().Set(self != nullptr ? roaring_bitmap_minimum(self->roaring) : 0);
}

void RoaringBitmap32::maximum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  return info.GetReturnValue().Set(self != nullptr ? roaring_bitmap_maximum(self->roaring) : 0);
}

void RoaringBitmap32::rank(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::select(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::removeRunCompression(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::runOptimize(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::shrinkToFit(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::freeze(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self && self->frozenCounter >= 0) {
    self->frozenCounter = FROZEN_COUNTER_SOFT_FROZEN;
  }
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::clone(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  v8::Local<v8::Value> argv[1] = {info.Holder()};
  auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
  v8::Local<v8::Object> vlocal;
  if (v.ToLocal(&vlocal)) {
    info.GetReturnValue().Set(vlocal);
  }
}

void RoaringBitmap32::statistics(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  roaring_statistics_t stats;
  roaring_bitmap_statistics(self->roaring, &stats);
  auto context = isolate->GetCurrentContext();
  auto result = v8::Object::New(isolate);
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "containers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "arrayContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_array_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "runContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_run_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bitsetContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bitset_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "valuesInArrayContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_values_array_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "valuesInRunContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_values_run_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "valuesInBitsetContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_values_bitset_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bytesInArrayContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_array_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bytesInRunContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_run_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "bytesInBitsetContainers", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.n_bytes_bitset_containers)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "maxValue", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.max_value)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "minValue", v8::NewStringType::kInternalized),
    v8::Uint32::NewFromUnsigned(isolate, stats.min_value)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "sumOfAllValues", v8::NewStringType::kInternalized),
    v8::Number::New(isolate, (double)stats.sum_value)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "size", v8::NewStringType::kInternalized),
    v8::Number::New(isolate, (double)stats.cardinality)));
  v8utils::ignoreMaybeResult(result->Set(
    context,
    NEW_LITERAL_V8_STRING(isolate, "isFrozen", v8::NewStringType::kInternalized),
    v8::Boolean::New(isolate, self->isFrozen())));
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::isSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self == other || (self && other && roaring_bitmap_is_subset(self->roaring, other->roaring)));
}

void RoaringBitmap32::isStrictSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self && other && roaring_bitmap_is_strict_subset(self->roaring, other->roaring));
}

void RoaringBitmap32::isEqual(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(
    self == other || (self && other && roaring_bitmap_equals(self->roaring, other->roaring) ? true : false));
}

void RoaringBitmap32::intersects(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self && other && roaring_bitmap_intersect(self->roaring, other->roaring) ? true : false);
}

void RoaringBitmap32::jaccardIndex(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self && other ? roaring_bitmap_jaccard_index(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::copyFrom(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::swapStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap expects 2 arguments");

  auto constructorTemplate = globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
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

void RoaringBitmap32::toString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  info.GetReturnValue().Set(NEW_LITERAL_V8_STRING(isolate, "RoaringBitmap32", v8::NewStringType::kInternalized));
}

void RoaringBitmap32::contentToString(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

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

void RoaringBitmap32::fromArrayStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Value> arg;

  if (info.Length() >= 1) {
    arg = info[0];
  }

  auto * worker = new FromArrayAsyncWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "Failed to allocate async worker");
  }

  if (!arg->IsNullOrUndefined() && !arg->IsFunction()) {
    if (arg->IsUint32Array() || arg->IsInt32Array()) {
      worker->buffer.set(isolate, arg);
      const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, arg);
    } else if (arg->IsObject()) {
      if (globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate)->HasInstance(arg)) {
        return v8utils::throwTypeError(
          isolate, "RoaringBitmap32::fromArrayAsync cannot be called with a RoaringBitmap32 instance");
      }
      v8::Local<v8::Value> argv[] = {arg};
      auto carg = globalAddonData.Uint32Array_from.Get(isolate)->Call(
        isolate->GetCurrentContext(), globalAddonData.Uint32Array.Get(isolate), 1, argv);
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

static void RoaringBitmap32_Init(v8::Local<v8::Object> exports) {
  croaringMemoryInitialize();

  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto className = NEW_LITERAL_V8_STRING(isolate, "RoaringBitmap32", v8::NewStringType::kInternalized);
  auto versionString = NEW_LITERAL_V8_STRING(isolate, ROARING_VERSION, v8::NewStringType::kInternalized);

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, RoaringBitmap32_New);
  if (ctor.IsEmpty()) {
    return;
  }
  globalAddonData.RoaringBitmap32_constructorTemplate.Set(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  v8::Local<v8::ObjectTemplate> ctorInstanceTemplate = ctor->InstanceTemplate();

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "isEmpty", v8::NewStringType::kInternalized),
    RoaringBitmap32::isEmpty_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "size", v8::NewStringType::kInternalized),
    RoaringBitmap32::size_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "isFrozen", v8::NewStringType::kInternalized),
    RoaringBitmap32::isFrozen_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  NODE_SET_PROTOTYPE_METHOD(ctor, "add", RoaringBitmap32::add);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addMany", RoaringBitmap32::addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addRange", RoaringBitmap32::addRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andCardinality", RoaringBitmap32::andCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andInPlace", RoaringBitmap32::andInPlace);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotCardinality", RoaringBitmap32::andNotCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotInPlace", RoaringBitmap32::removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "asReadonlyView", RoaringBitmap32::asReadonlyView);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clear", RoaringBitmap32::clear);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clone", RoaringBitmap32::clone);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contains", RoaringBitmap32::has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "containsRange", RoaringBitmap32::hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contentToString", RoaringBitmap32::contentToString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "copyFrom", RoaringBitmap32::copyFrom);
  NODE_SET_PROTOTYPE_METHOD(ctor, "delete", RoaringBitmap32::removeChecked);
  NODE_SET_PROTOTYPE_METHOD(ctor, "deserialize", RoaringBitmap32::deserialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "flipRange", RoaringBitmap32::flipRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "freeze", RoaringBitmap32::freeze);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSerializationSizeInBytes", RoaringBitmap32::getSerializationSizeInBytes);
  NODE_SET_PROTOTYPE_METHOD(ctor, "has", RoaringBitmap32::has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "hasRange", RoaringBitmap32::hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "intersects", RoaringBitmap32::intersects);
  NODE_SET_PROTOTYPE_METHOD(ctor, "intersectsWithRange", RoaringBitmap32::intersectsWithRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isEqual", RoaringBitmap32::isEqual);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isStrictSubset", RoaringBitmap32::isStrictSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isSubset", RoaringBitmap32::isSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "jaccardIndex", RoaringBitmap32::jaccardIndex);
  NODE_SET_PROTOTYPE_METHOD(ctor, "maximum", RoaringBitmap32::maximum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "minimum", RoaringBitmap32::minimum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orCardinality", RoaringBitmap32::orCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orInPlace", RoaringBitmap32::addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rangeCardinality", RoaringBitmap32::rangeCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rangeUint32Array", RoaringBitmap32::rangeUint32Array);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rank", RoaringBitmap32::rank);
  NODE_SET_PROTOTYPE_METHOD(ctor, "remove", RoaringBitmap32::remove);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeMany", RoaringBitmap32::removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRange", RoaringBitmap32::removeRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRunCompression", RoaringBitmap32::removeRunCompression);
  NODE_SET_PROTOTYPE_METHOD(ctor, "runOptimize", RoaringBitmap32::runOptimize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "select", RoaringBitmap32::select);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serialize", RoaringBitmap32::serialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serializeAsync", RoaringBitmap32::serializeAsync);
  NODE_SET_PROTOTYPE_METHOD(ctor, "shrinkToFit", RoaringBitmap32::shrinkToFit);
  NODE_SET_PROTOTYPE_METHOD(ctor, "statistics", RoaringBitmap32::statistics);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toArray", RoaringBitmap32::toArray);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toSet", RoaringBitmap32::toSet);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toString", RoaringBitmap32::toString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toUint32Array", RoaringBitmap32::toUint32Array);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toUint32ArrayAsync", RoaringBitmap32::toUint32ArrayAsync);
  NODE_SET_PROTOTYPE_METHOD(ctor, "tryAdd", RoaringBitmap32::tryAdd);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorCardinality", RoaringBitmap32::xorCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorInPlace", RoaringBitmap32::xorInPlace);

  auto context = isolate->GetCurrentContext();
  auto ctorFunction = ctor->GetFunction(context).ToLocalChecked();
  auto ctorObject = ctorFunction->ToObject(context).ToLocalChecked();

  ctor->PrototypeTemplate()->Set(v8::Symbol::GetToStringTag(isolate), versionString);
  ctor->PrototypeTemplate()->Set(isolate, "CRoaringVersion", versionString);

  NODE_SET_METHOD(ctorObject, "addOffset", RoaringBitmap32::addOffsetStatic);
  NODE_SET_METHOD(ctorObject, "and", RoaringBitmap32::andStatic);
  NODE_SET_METHOD(ctorObject, "andNot", RoaringBitmap32::andNotStatic);

  v8utils::defineHiddenField(isolate, ctorObject, "default", ctorFunction);

  NODE_SET_METHOD(ctorObject, "deserialize", RoaringBitmap32::deserializeStatic);
  NODE_SET_METHOD(ctorObject, "deserializeAsync", RoaringBitmap32::deserializeStaticAsync);
  NODE_SET_METHOD(ctorObject, "deserializeParallelAsync", RoaringBitmap32::deserializeParallelStaticAsync);

  v8utils::ignoreMaybeResult(
    ctorObject->Set(context, NEW_LITERAL_V8_STRING(isolate, "from", v8::NewStringType::kInternalized), ctorFunction));

  NODE_SET_METHOD(ctorObject, "fromArrayAsync", RoaringBitmap32::fromArrayStaticAsync);
  NODE_SET_METHOD(ctorObject, "fromRange", RoaringBitmap32::fromRangeStatic);
  NODE_SET_METHOD(ctorObject, "getInstancesCount", RoaringBitmap32::getInstanceCountStatic);
  NODE_SET_METHOD(ctorObject, "or", RoaringBitmap32::orStatic);
  NODE_SET_METHOD(ctorObject, "orMany", RoaringBitmap32::orManyStatic);
  NODE_SET_METHOD(ctorObject, "swap", RoaringBitmap32::swapStatic);
  NODE_SET_METHOD(ctorObject, "unsafeFrozenView", RoaringBitmap32::unsafeFrozenViewStatic);
  NODE_SET_METHOD(ctorObject, "xor", RoaringBitmap32::xorStatic);
  NODE_SET_METHOD(ctorObject, "xorMany", RoaringBitmap32::xorManyStatic);

  v8utils::defineReadonlyField(isolate, ctorObject, "CRoaringVersion", versionString);
  v8utils::defineReadonlyField(isolate, exports, "CRoaringVersion", versionString);

  v8utils::defineHiddenField(isolate, ctorObject, "RoaringBitmap32", ctorFunction);

  v8utils::ignoreMaybeResult(exports->Set(context, className, ctorFunction));

  globalAddonData.RoaringBitmap32_constructor.Set(isolate, ctorFunction);
}

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include <limits>
#include <string>

/////////////////// unity build ///////////////////

#include "v8utils.cpp"

#define printf(...) ((void)0)
#define fprintf(...) ((void)0)

#include "RoaringBitmap32.h"

#define CROARING_SERIALIZATION_ARRAY_UINT32 1
#define CROARING_SERIALIZATION_CONTAINER 2

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#  pragma clang diagnostic ignored "-Wunused-but-set-variable"
#  pragma clang diagnostic ignored "-Wunused-but-set-parameter"
#  pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

#ifdef small
#  undef small  // on windows this seems to be defined to something...
#endif

#include "CRoaringUnityBuild/roaring.c"

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif

#undef printf
#undef fprintf

static void buffer_aligned_free_callback(char * data, void * hint) { gcaware_aligned_free(data); }

/////////////////// RoaringBitmap32 ///////////////////

const uint32_t MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES = 0x00FFFFFF;

std::atomic<uint64_t> RoaringBitmap32InstancesCounter{0};

void initTypes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  JSTypes::initJSTypes(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());

  roaring_memory_t roaringMemory;
  roaringMemory.malloc = gcaware_malloc;
  roaringMemory.realloc = gcaware_realloc;
  roaringMemory.calloc = gcaware_calloc;
  roaringMemory.free = gcaware_free;
  roaringMemory.aligned_malloc = gcaware_aligned_malloc;
  roaringMemory.aligned_free = gcaware_aligned_free;

  roaring_init_memory_hook(roaringMemory);
}

void _bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info, bool unsafe) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  int64_t size;
  int32_t alignment = 32;
  if (
    info.Length() < 1 || !info[0]->IsNumber() || !info[0]->IntegerValue(isolate->GetCurrentContext()).To(&size) ||
    size < 0) {
    return v8utils::throwTypeError(isolate, "Buffer size must be a positive integer");
  }

  if (info.Length() >= 2 && !info[1]->IsUndefined()) {
    if (!info[1]->IsNumber() || !info[1]->Int32Value(isolate->GetCurrentContext()).To(&alignment) || alignment <= 0) {
      return v8utils::throwTypeError(isolate, "Buffer alignment must be a positive integer");
    }
  }

  if ((uint64_t)size > v8::TypedArray::kMaxLength || (uint64_t)size + alignment >= v8::TypedArray::kMaxLength) {
    return v8utils::throwTypeError(isolate, "Buffer size is too large");
  }

  if (alignment > 1024) {
    return v8utils::throwTypeError(isolate, "Buffer alignment is too large");
  }

  void * ptr = bare_aligned_malloc(alignment, size);
  if (!ptr) {
    return v8utils::throwError(isolate, "Buffer memory allocation failed");
  }

  if (!unsafe) {
    memset(ptr, 0, size);
  }

  v8::MaybeLocal<v8::Object> bufferMaybe =
    node::Buffer::New(isolate, (char *)ptr, size, buffer_aligned_free_callback, nullptr);

  v8::Local<v8::Object> bufferObj;
  if (!bufferMaybe.ToLocal(&bufferObj)) {
    bare_aligned_free(ptr);
    return v8utils::throwError(isolate, "Buffer creation failed");
  }

  info.GetReturnValue().Set(bufferObj);
}

void bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufferAlignedAlloc(info, false); }

void bufferAlignedAllocUnsafe(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufferAlignedAlloc(info, true); }

inline bool is_pointer_aligned(const void * ptr, std::uintptr_t alignment) noexcept {
  auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
  return !(iptr % alignment);
}

void isBufferAligned(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1) {
    return info.GetReturnValue().Set(false);
  }

  // Second parameter must be a positive integer
  int32_t alignment = 32;
  if (info.Length() >= 2 && !info[1]->IsUndefined()) {
    if (!info[1]->IsNumber() || !info[1]->Int32Value(isolate->GetCurrentContext()).To(&alignment) || alignment < 0) {
      return info.GetReturnValue().Set(false);
    }
  }

  if (alignment == 0) {
    return info.GetReturnValue().Set(true);
  }

  v8utils::TypedArrayContent<uint8_t> content(isolate, info[0]);
  info.GetReturnValue().Set(is_pointer_aligned(content.data, alignment));
}

void getRoaringUsedMemory(const v8::FunctionCallbackInfo<v8::Value> & info) {
  info.GetReturnValue().Set((double)gcaware_totalMem());
}

void InitModule(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  JSTypes::initJSTypes(isolate, isolate->GetCurrentContext()->Global());

  v8utils::defineHiddenFunction(isolate, exports, "_initTypes", initTypes);
  v8utils::defineHiddenField(isolate, exports, "default", exports);

  NODE_SET_METHOD(exports, "bufferAlignedAlloc", bufferAlignedAlloc);
  NODE_SET_METHOD(exports, "bufferAlignedAllocUnsafe", bufferAlignedAllocUnsafe);
  NODE_SET_METHOD(exports, "isBufferAligned", isBufferAligned);
  NODE_SET_METHOD(exports, "getRoaringUsedMemory", getRoaringUsedMemory);

  RoaringBitmap32::Init(exports);
  RoaringBitmap32BufferedIterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

/////// enum parsing ///////

static SerializationFormat tryParseSerializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  if (!isolate || value.IsEmpty()) {
    return SerializationFormat::INVALID;
  }
  if (value->IsBoolean()) {
    if (value->IsTrue()) {
      return SerializationFormat::portable;
    }
    if (value->IsFalse()) {
      return SerializationFormat::croaring;
    }
  } else if (value->IsString()) {
    v8::String::Utf8Value formatString(isolate, value);
    if (strcmp(*formatString, "croaring") == 0) {
      return SerializationFormat::croaring;
    }
    if (strcmp(*formatString, "portable") == 0) {
      return SerializationFormat::portable;
    }
    if (strcmp(*formatString, "unsafe_frozen_croaring") == 0) {
      return SerializationFormat::unsafe_frozen_croaring;
    }
  }
  return SerializationFormat::INVALID;
}

static DeserializationFormat tryParseDeserializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  if (!isolate || value.IsEmpty()) {
    return DeserializationFormat::INVALID;
  }
  if (value->IsBoolean()) {
    if (value->IsTrue()) {
      return DeserializationFormat::portable;
    }
    if (value->IsFalse()) {
      return DeserializationFormat::croaring;
    }
  } else if (value->IsString()) {
    v8::String::Utf8Value formatString(isolate, value);
    if (strcmp(*formatString, "croaring") == 0) {
      return DeserializationFormat::croaring;
    }
    if (strcmp(*formatString, "portable") == 0) {
      return DeserializationFormat::portable;
    }
    if (strcmp(*formatString, "unsafe_frozen_croaring") == 0) {
      return DeserializationFormat::unsafe_frozen_croaring;
    }
    if (strcmp(*formatString, "unsafe_frozen_portable") == 0) {
      return DeserializationFormat::unsafe_frozen_portable;
    }
  }
  return DeserializationFormat::INVALID;
}

static FrozenViewFormat tryParseFrozenViewFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  if (!isolate || value.IsEmpty()) {
    return FrozenViewFormat::INVALID;
  }
  if (value->IsString()) {
    v8::String::Utf8Value formatString(isolate, value);
    if (strcmp(*formatString, "unsafe_frozen_croaring") == 0) {
      return FrozenViewFormat::unsafe_frozen_croaring;
    }
    if (strcmp(*formatString, "unsafe_frozen_portable") == 0) {
      return FrozenViewFormat::unsafe_frozen_portable;
    }
  }
  return FrozenViewFormat::INVALID;
}

//////////// RoaringBitmap32 ////////////

const char * const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";
const char * const ERROR_INVALID_OBJECT = "Invalid RoaringBitmap32 object";

v8::Eternal<v8::FunctionTemplate> RoaringBitmap32::constructorTemplate;
v8::Eternal<v8::Function> RoaringBitmap32::constructor;

void RoaringBitmap32::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto className = NEW_LITERAL_V8_STRING(isolate, "RoaringBitmap32", v8::NewStringType::kInternalized);
  auto versionString = NEW_LITERAL_V8_STRING(isolate, ROARING_VERSION, v8::NewStringType::kInternalized);

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, RoaringBitmap32::New);
  RoaringBitmap32::constructorTemplate.Set(isolate, ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(className);

  v8::Local<v8::ObjectTemplate> ctorInstanceTemplate = ctor->InstanceTemplate();

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "isEmpty", v8::NewStringType::kInternalized),
    isEmpty_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "size", v8::NewStringType::kInternalized),
    size_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  ctorInstanceTemplate->SetAccessor(
    NEW_LITERAL_V8_STRING(isolate, "isFrozen", v8::NewStringType::kInternalized),
    isFrozen_getter,
    nullptr,
    v8::Local<v8::Value>(),
    (v8::AccessControl)(v8::ALL_CAN_READ | v8::PROHIBITS_OVERWRITING),
    (v8::PropertyAttribute)(v8::ReadOnly));

  NODE_SET_PROTOTYPE_METHOD(ctor, "add", add);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addMany", addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "addRange", addRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andCardinality", andCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andInPlace", andInPlace);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotCardinality", andNotCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "andNotInPlace", removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clear", clear);
  NODE_SET_PROTOTYPE_METHOD(ctor, "clone", clone);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contains", has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "containsRange", hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "contentToString", contentToString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "copyFrom", copyFrom);
  NODE_SET_PROTOTYPE_METHOD(ctor, "delete", removeChecked);
  NODE_SET_PROTOTYPE_METHOD(ctor, "deserialize", deserialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "flipRange", flipRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "freeze", freeze);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSerializationSizeInBytes", getSerializationSizeInBytes);
  NODE_SET_PROTOTYPE_METHOD(ctor, "has", has);
  NODE_SET_PROTOTYPE_METHOD(ctor, "hasRange", hasRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "intersects", intersects);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isEqual", isEqual);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isStrictSubset", isStrictSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isSubset", isSubset);
  NODE_SET_PROTOTYPE_METHOD(ctor, "jaccardIndex", jaccardIndex);
  NODE_SET_PROTOTYPE_METHOD(ctor, "maximum", maximum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "minimum", minimum);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orCardinality", orCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "orInPlace", addMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rangeCardinality", rangeCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rangeUint32Array", rangeUint32Array);
  NODE_SET_PROTOTYPE_METHOD(ctor, "rank", rank);
  NODE_SET_PROTOTYPE_METHOD(ctor, "remove", remove);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeMany", removeMany);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRange", removeRange);
  NODE_SET_PROTOTYPE_METHOD(ctor, "removeRunCompression", removeRunCompression);
  NODE_SET_PROTOTYPE_METHOD(ctor, "runOptimize", runOptimize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "select", select);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serialize", serialize);
  NODE_SET_PROTOTYPE_METHOD(ctor, "serializeAsync", serializeAsync);
  NODE_SET_PROTOTYPE_METHOD(ctor, "shrinkToFit", shrinkToFit);
  NODE_SET_PROTOTYPE_METHOD(ctor, "statistics", statistics);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toArray", toArray);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toSet", toSet);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toString", toString);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toUint32Array", toUint32Array);
  NODE_SET_PROTOTYPE_METHOD(ctor, "toUint32ArrayAsync", toUint32ArrayAsync);
  NODE_SET_PROTOTYPE_METHOD(ctor, "tryAdd", tryAdd);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorCardinality", xorCardinality);
  NODE_SET_PROTOTYPE_METHOD(ctor, "xorInPlace", xorInPlace);

  auto context = isolate->GetCurrentContext();
  auto ctorFunction = ctor->GetFunction(context).ToLocalChecked();
  auto ctorObject = ctorFunction->ToObject(context).ToLocalChecked();

  ctor->PrototypeTemplate()->Set(v8::Symbol::GetToStringTag(isolate), versionString);
  ctor->PrototypeTemplate()->Set(isolate, "CRoaringVersion", versionString);

  NODE_SET_METHOD(ctorObject, "addOffset", addOffsetStatic);
  NODE_SET_METHOD(ctorObject, "and", andStatic);
  NODE_SET_METHOD(ctorObject, "andNot", andNotStatic);

  v8utils::defineHiddenField(isolate, ctorObject, "default", ctorFunction);

  NODE_SET_METHOD(ctorObject, "deserialize", deserializeStatic);
  NODE_SET_METHOD(ctorObject, "deserializeAsync", deserializeStaticAsync);
  NODE_SET_METHOD(ctorObject, "deserializeParallelAsync", deserializeParallelStaticAsync);

  v8utils::ignoreMaybeResult(
    ctorObject->Set(context, NEW_LITERAL_V8_STRING(isolate, "from", v8::NewStringType::kInternalized), ctorFunction));

  NODE_SET_METHOD(ctorObject, "fromArrayAsync", fromArrayStaticAsync);
  NODE_SET_METHOD(ctorObject, "fromRange", fromRangeStatic);
  NODE_SET_METHOD(ctorObject, "getInstancesCount", getInstanceCountStatic);
  NODE_SET_METHOD(ctorObject, "or", orStatic);
  NODE_SET_METHOD(ctorObject, "orMany", orManyStatic);
  NODE_SET_METHOD(ctorObject, "swap", swapStatic);
  NODE_SET_METHOD(ctorObject, "unsafeFrozenView", unsafeFrozenViewStatic);
  NODE_SET_METHOD(ctorObject, "xor", xorStatic);
  NODE_SET_METHOD(ctorObject, "xorMany", xorManyStatic);

  v8utils::defineReadonlyField(isolate, ctorObject, "CRoaringVersion", versionString);
  v8utils::defineReadonlyField(isolate, exports, "CRoaringVersion", versionString);

  v8utils::defineHiddenField(isolate, ctorObject, "RoaringBitmap32", ctorFunction);

  v8utils::ignoreMaybeResult(exports->Set(context, className, ctorFunction));

  constructor.Set(isolate, ctorFunction);
}

RoaringBitmap32::RoaringBitmap32(uint32_t capacity) :
  roaring(roaring_bitmap_create_with_capacity(capacity)), version(0), frozenCounter(0) {
  gcaware_adjustAllocatedMemory(sizeof(RoaringBitmap32));
  ++RoaringBitmap32InstancesCounter;
}

RoaringBitmap32::~RoaringBitmap32() {
  --RoaringBitmap32InstancesCounter;
  if (this->roaring != nullptr) {
    roaring_bitmap_free(this->roaring);
  }
  if (this->frozenStorage.data != nullptr && this->frozenStorage.length == std::numeric_limits<size_t>::max()) {
    gcaware_aligned_free(this->frozenStorage.data);
  }
  this->persistent.Reset();
  gcaware_adjustAllocatedMemory(-sizeof(RoaringBitmap32));
}

void RoaringBitmap32::getInstanceCountStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  info.GetReturnValue().Set((double)RoaringBitmap32InstancesCounter);
}

bool RoaringBitmap32::replaceBitmapInstance(v8::Isolate * isolate, roaring_bitmap_t * newInstance) {
  if (this->roaring != newInstance) {
    if (this->roaring != nullptr) {
      roaring_bitmap_free(this->roaring);
      this->roaring = nullptr;
    }
    this->roaring = newInstance;
    this->invalidate();
    return true;
  }
  return false;
}

void RoaringBitmap32::WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32> const & info) {
  RoaringBitmap32 * p = info.GetParameter();
  if (p != nullptr) {
    delete p;
  }
}

void RoaringBitmap32::New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    v8::Local<v8::Function> cons = constructor.Get(isolate);
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

  RoaringBitmap32 * instance;
  instance = new RoaringBitmap32(0);
  if (instance == nullptr || instance->roaring == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32::ctor - failed to create native RoaringBitmap32 instance");
  }

  holder->SetAlignedPointerInInternalField(0, instance);
  instance->persistent.Reset(isolate, holder);
  instance->persistent.SetWeak(instance, WeakCallback, v8::WeakCallbackType::kParameter);

  bool hasParameter = info.Length() != 0 && !info[0]->IsUndefined() && !info[0]->IsNull();
  if (hasParameter) {
    if (RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0])) {
      RoaringBitmap32::copyFrom(info);
    } else {
      RoaringBitmap32::addMany(info);
    }
  }

  info.GetReturnValue().Set(holder);
}

void RoaringBitmap32::size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (self == nullptr) {
    return info.GetReturnValue().Set(0U);
  }

  size_t size = roaring_bitmap_get_cardinality(self->roaring);
  if (size <= 0xFFFFFFFF) {
    return info.GetReturnValue().Set((uint32_t)size);
  }

  info.GetReturnValue().Set((double)size);
}

void RoaringBitmap32::isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (self == nullptr) {
    return info.GetReturnValue().Set(true);
  }
  info.GetReturnValue().Set(roaring_bitmap_is_empty(self->roaring) ? true : false);
}

void RoaringBitmap32::isFrozen_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info) {
  const RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (self == nullptr) {
    return info.GetReturnValue().Set(true);
  }
  info.GetReturnValue().Set(self->isFrozen());
}

void RoaringBitmap32::has(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  const RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<const RoaringBitmap32>(info.Holder(), isolate);
  uint32_t v;
  if (
    self == nullptr || info.Length() < 1 || !info[0]->IsUint32() ||
    !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    info.GetReturnValue().Set(false);
  } else {
    info.GetReturnValue().Set(roaring_bitmap_contains(self->roaring, v));
  }
}

void RoaringBitmap32::hasRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return info.GetReturnValue().Set(false);
  }

  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  double minimum, maximum;
  if (
    info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[0]->NumberValue(context).To(&minimum) ||
    !info[1]->NumberValue(context).To(&maximum)) {
    return info.GetReturnValue().Set(false);
  }

  if (std::isnan(minimum) || std::isnan(maximum)) {
    return info.GetReturnValue().Set(false);
  }

  minimum = ceil(minimum);
  maximum = ceil(maximum);
  if (minimum < 0 || maximum > 4294967296) {
    return info.GetReturnValue().Set(false);
  }

  uint64_t minInteger = (uint64_t)minimum;
  uint64_t maxInteger = (uint64_t)maximum;

  if (minInteger >= maxInteger || maxInteger > 4294967296) {
    return info.GetReturnValue().Set(false);
  }

  info.GetReturnValue().Set(roaring_bitmap_contains_range(self->roaring, minInteger, maxInteger));
}

void RoaringBitmap32::minimum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  return info.GetReturnValue().Set(self != nullptr ? roaring_bitmap_minimum(self->roaring) : 0);
}

void RoaringBitmap32::maximum(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  return info.GetReturnValue().Set(self != nullptr ? roaring_bitmap_maximum(self->roaring) : 0);
}

void RoaringBitmap32::rank(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozenHard()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  info.GetReturnValue().Set((double)roaring_bitmap_shrink_to_fit(self->roaring));
}

void RoaringBitmap32::toUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  auto size = static_cast<size_t>(roaring_bitmap_get_cardinality(self->roaring));

  v8utils::TypedArrayContent<uint32_t> typedArrayContent;

  // Check if the first argument is a UInt32Array or a Int32Array
  if (info.Length() >= 1 && !info[0]->IsUndefined()) {
    v8::Local<v8::Object> obj;
    if (
      !info[0]->IsObject() || !info[0]->ToObject(isolate->GetCurrentContext()).ToLocal(&obj) ||
      (!obj->IsUint32Array() && !obj->IsInt32Array() && !obj->IsArrayBuffer()) || !typedArrayContent.set(isolate, obj)) {
      return v8utils::throwError(
        isolate, "RoaringBitmap32::toUint32Array - argument must be a UInt32Array, Int32Array or ArrayBuffer");
    }
    if (size > typedArrayContent.length) {
      return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - the array is too small");
    }
    if (size != 0) {
      roaring_bitmap_to_uint32_array(self->roaring, typedArrayContent.data);
    }
    return info.GetReturnValue().Set(obj);
  }

  auto arrayBuffer = v8::ArrayBuffer::New(isolate, size * sizeof(uint32_t));
  auto typedArray = v8::Uint32Array::New(arrayBuffer, 0, size);

  if (size != 0) {
    if (!typedArrayContent.set(isolate, typedArray))
      return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - failed to allocate memory");
    roaring_bitmap_to_uint32_array(self->roaring, typedArrayContent.data);
  }

  info.GetReturnValue().Set(typedArray);
}

class ToUint32ArrayAsyncWorker final : public v8utils::AsyncWorker {
 public:
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bitmapPersistent;
  RoaringBitmap32 * bitmap = nullptr;
  v8utils::TypedArrayContent<uint32_t> inputContent;
  uint32_t * volatile allocatedBuffer = nullptr;
  size_t volatile outputSize = 0;
  bool hasInput = false;

  explicit ToUint32ArrayAsyncWorker(v8::Isolate * isolate) : v8utils::AsyncWorker(isolate) {
    gcaware_adjustAllocatedMemory(sizeof(ToUint32ArrayAsyncWorker));
  }

  ~ToUint32ArrayAsyncWorker() {
    if (this->allocatedBuffer) {
      bare_aligned_free(this->allocatedBuffer);
    }
    gcaware_adjustAllocatedMemory(-sizeof(ToUint32ArrayAsyncWorker));
  }

  void parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Isolate * isolate = info.GetIsolate();

    RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
    if (self == nullptr) {
      return this->setError(ERROR_INVALID_OBJECT);
    }
    this->bitmap = self;

    // Check if the first argument is a UInt32Array or a Int32Array
    if (info.Length() >= 1 && !info[0]->IsUndefined()) {
      v8::Local<v8::Object> obj;
      if (
        !info[0]->IsObject() || !info[0]->ToObject(isolate->GetCurrentContext()).ToLocal(&obj) ||
        (!obj->IsUint32Array() && !obj->IsInt32Array() && !obj->IsArrayBuffer()) || !this->inputContent.set(isolate, obj)) {
        return this->setError(
          "RoaringBitmap32::toUint32ArrayAsync - argument must be a UInt32Array, Int32Array or ArrayBuffer");
      }
      this->hasInput = true;
    }
  }

 protected:
  // Called before the thread starts, in the main thread.
  void before() final {
    if (this->bitmap) {
      this->bitmap->beginFreeze();
    }
  }

  void work() final {
    auto size = static_cast<size_t>(roaring_bitmap_get_cardinality(this->bitmap->roaring));
    if (size == 0) {
      return;
    }

    this->outputSize = size;

    if (this->hasInput) {
      if (size > this->inputContent.length) {
        return this->setError("RoaringBitmap32::toUint32ArrayAsync - the array is too small");
      }
      roaring_bitmap_to_uint32_array(this->bitmap->roaring, this->inputContent.data);
      return;
    }

    // Allocate a new buffer
    this->allocatedBuffer = (uint32_t *)bare_aligned_malloc(32, size * sizeof(uint32_t));
    if (!this->allocatedBuffer) {
      return this->setError("RoaringBitmap32::toUint32ArrayAsync - failed to allocate memory");
    }
    roaring_bitmap_to_uint32_array(this->bitmap->roaring, this->allocatedBuffer);
  }

  void finally() final {
    if (this->bitmap) {
      this->bitmap->endFreeze();
    }
  }

  v8::Local<v8::Value> done() final {
    v8::EscapableHandleScope scope(this->isolate);
    uint32_t * allocatedBuffer = this->allocatedBuffer;

    if (this->hasInput) {
      return this->inputContent.bufferPersistent.Get(isolate);
    }

    if (allocatedBuffer && this->outputSize != 0) {
      // Create a new buffer using the allocated memory
      v8::MaybeLocal<v8::Object> nodeBufferMaybeLocal = node::Buffer::New(
        isolate, (char *)allocatedBuffer, this->outputSize * sizeof(uint32_t), buffer_aligned_free_callback, nullptr);
      if (!nodeBufferMaybeLocal.IsEmpty()) {
        this->allocatedBuffer = nullptr;
      }

      v8::Local<v8::Object> nodeBufferObject;
      if (!nodeBufferMaybeLocal.ToLocal(&nodeBufferObject)) {
        this->setError("RoaringBitmap32::toUint32ArrayAsync - failed to create a new buffer");
        return scope.Escape(v8::Local<v8::Value>());
      }

      v8::Local<v8::Uint8Array> nodeBuffer = nodeBufferObject.As<v8::Uint8Array>();
      return scope.Escape(v8::Uint32Array::New(nodeBuffer->Buffer(), 0, this->outputSize));
    }

    return scope.Escape(v8::Uint32Array::New(v8::ArrayBuffer::New(isolate, 0), 0, 0));
  }
};

void RoaringBitmap32::toUint32ArrayAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  ToUint32ArrayAsyncWorker * worker = new ToUint32ArrayAsyncWorker(isolate);
  if (!worker) {
    return v8utils::throwError(isolate, "RoaringBitmap32::toUint32ArrayAsync - allocation failed");
  }
  worker->parseArguments(info);
  info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
}

void RoaringBitmap32::rangeUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  uint32_t offset;
  uint32_t limit;
  if (info.Length() < 2 || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&offset)) {
    return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - first argument must be a valid uint32");
  }

  if (info.Length() < 2 || !info[1]->Uint32Value(isolate->GetCurrentContext()).To(&limit)) {
    return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - second argument must be a valid uint32");
  }

  auto cardinality = roaring_bitmap_get_cardinality(self->roaring);
  auto size = limit < cardinality - offset ? limit : cardinality - offset;

  if (offset > cardinality) {
    size = 0;
  }

  if (limit > cardinality - offset) {
    limit = cardinality - offset;
  }

  v8utils::TypedArrayContent<uint32_t> typedArrayContent;

  // Check if the first argument is a UInt32Array or a Int32Array
  if (info.Length() >= 1 && !info[2]->IsUndefined()) {
    v8::Local<v8::Object> obj;
    if (
      !info[2]->ToObject(isolate->GetCurrentContext()).ToLocal(&obj) ||
      (!obj->IsUint32Array() && !obj->IsInt32Array() && !obj->IsArrayBuffer()) || !typedArrayContent.set(isolate, obj)) {
      return v8utils::throwError(
        isolate, "RoaringBitmap32::rangeUint32Array - argument must be a UInt32Array, Int32Array or ArrayBuffer");
    }
    if (size > typedArrayContent.length) {
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - the array is too small");
    }
    roaring_bitmap_range_uint32_array(self->roaring, offset, limit, typedArrayContent.data);
    return info.GetReturnValue().Set(obj);
  }

  auto arrayBuffer = v8::ArrayBuffer::New(isolate, size * sizeof(uint32_t));
  auto typedArray = v8::Uint32Array::New(arrayBuffer, 0, size);

  if (size != 0) {
    if (!typedArrayContent.set(isolate, typedArray))
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - failed to allocate memory");

    roaring_bitmap_range_uint32_array(self->roaring, offset, limit, typedArrayContent.data);
  }

  info.GetReturnValue().Set(typedArray);
}

void RoaringBitmap32::toArray(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  size_t cardinality = self ? roaring_bitmap_get_cardinality(self->roaring) : 0;

  struct iter_data {
    uint32_t index;
    uint32_t count;
    double maxLength;
    v8::Isolate * isolate;
    v8::Local<v8::Context> context;
    v8::Local<v8::Array> jsArray;
  } iterData;

  iterData.index = 0;
  iterData.count = 0;
  iterData.isolate = isolate;
  iterData.context = isolate->GetCurrentContext();

  double maxLength = cardinality;

  // Check if the first argument is an array
  if (info.Length() >= 1 && !info[0]->IsUndefined()) {
    if (info[0]->IsNumber()) {
      if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&maxLength)) {
        return v8utils::throwTypeError(isolate, "RoaringBitmap32::toArray - maxLength argument must be a valid number");
      }
      if (maxLength > cardinality) {
        maxLength = cardinality;
      }
      iterData.jsArray = v8::Array::New(isolate, (uint32_t)maxLength);
    } else {
      v8::Local<v8::Object> obj;
      if (!info[0]->ToObject(isolate->GetCurrentContext()).ToLocal(&obj) || !obj->IsArray()) {
        return v8utils::throwTypeError(isolate, "RoaringBitmap32::toArray - argument must be an array");
      }
      iterData.jsArray = v8::Local<v8::Array>::Cast(obj);

      if (info.Length() >= 2 && !info[1]->IsUndefined()) {
        if (
          !info[1]->IsNumber() || !info[1]->NumberValue(isolate->GetCurrentContext()).To(&maxLength) ||
          std::isnan(maxLength)) {
          return v8utils::throwTypeError(
            isolate, "RoaringBitmap32::toArray - maxLength must be zero or a valid uint32 value");
        }
        if (maxLength > cardinality) {
          maxLength = cardinality;
        }
      }

      int64_t offset = 0;
      if (info.Length() >= 3 && !info[2]->IsUndefined()) {
        if (
          !info[2]->IsNumber() || !info[2]->IntegerValue(isolate->GetCurrentContext()).To(&offset) || offset < 0 ||
          offset > 4294967295) {
          return v8utils::throwTypeError(isolate, "RoaringBitmap32::toArray - offset must be zero or a valid uint32 value");
        }
        iterData.index = (uint32_t)offset;
      } else {
        iterData.index = iterData.jsArray->Length();
      }
    }
  } else {
    iterData.jsArray = v8::Array::New(isolate, cardinality);
  }

  iterData.maxLength = maxLength;

  info.GetReturnValue().Set(iterData.jsArray);

  if (cardinality != 0) {
    roaring_iterate(
      self->roaring,
      [](uint32_t value, void * vp) -> bool {
        auto * p = reinterpret_cast<iter_data *>(vp);
        bool r = false;
        return (p->count++) < p->maxLength &&
          p->jsArray->Set(p->context, p->index++, v8::Uint32::NewFromUnsigned(p->isolate, value)).To(&r) && r;
      },
      (void *)&iterData);
  }
}

void RoaringBitmap32::toSet(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  struct iter_data {
    uint64_t count;
    double maxLength;
    v8::Isolate * isolate;
    v8::Local<v8::Context> context;
    v8::Local<v8::Set> jsSet;
  } iterData;

  iterData.count = 0;
  iterData.maxLength = std::numeric_limits<double>::infinity();
  iterData.isolate = isolate;
  iterData.context = isolate->GetCurrentContext();

  if (info.Length() >= 1 && !info[0]->IsUndefined()) {
    if (info[0]->IsNumber()) {
      if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&iterData.maxLength)) {
        return v8utils::throwTypeError(isolate, "RoaringBitmap32::toSet - maxLength argument must be a valid number");
      }
      iterData.jsSet = v8::Set::New(isolate);
    } else {
      v8::Local<v8::Object> obj;
      if (!info[0]->ToObject(isolate->GetCurrentContext()).ToLocal(&obj) || !obj->IsSet()) {
        return v8utils::throwError(isolate, "RoaringBitmap32::toSet - argument must be a Set");
      }

      iterData.jsSet = v8::Local<v8::Set>::Cast(obj);

      if (info.Length() >= 2 && !info[1]->IsUndefined()) {
        if (!info[1]->IsNumber() || !info[1]->NumberValue(isolate->GetCurrentContext()).To(&iterData.maxLength)) {
          return v8utils::throwTypeError(isolate, "RoaringBitmap32::toSet - maxLength argument must be a valid number");
        }
      }
    }
  } else {
    iterData.jsSet = v8::Set::New(isolate);
  }

  if (std::isnan(iterData.maxLength)) {
    iterData.maxLength = std::numeric_limits<double>::infinity();
  }

  info.GetReturnValue().Set(iterData.jsSet);

  if (self != nullptr && !roaring_bitmap_is_empty(self->roaring)) {
    roaring_iterate(
      self->roaring,
      [](uint32_t value, void * vp) -> bool {
        auto * p = reinterpret_cast<iter_data *>(vp);
        return p->count++ < p->maxLength &&
          !p->jsSet->Add(p->context, v8::Uint32::NewFromUnsigned(p->isolate, value)).IsEmpty();
      },
      (void *)&iterData);
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

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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

  if (self != nullptr && !roaring_bitmap_is_empty(self->roaring)) {
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

void RoaringBitmap32::freeze(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self && self->frozenCounter >= 0) {
    self->frozenCounter = FROZEN_COUNTER_SOFT_FROZEN;
  }
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::clone(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = constructor.Get(isolate);

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

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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

//////////// RoaringBitmap32FactoryAsyncWorker ////////////

RoaringBitmap32FactoryAsyncWorker::RoaringBitmap32FactoryAsyncWorker(v8::Isolate * isolate) :
  v8utils::AsyncWorker(isolate), bitmap(nullptr) {}

RoaringBitmap32FactoryAsyncWorker::~RoaringBitmap32FactoryAsyncWorker() {
  if (this->bitmap != nullptr) {
    roaring_bitmap_aligned_free(this->bitmap);
    this->bitmap = nullptr;
  }
}

v8::Local<v8::Value> RoaringBitmap32FactoryAsyncWorker::done() {
  v8::EscapableHandleScope scope(this->isolate);
  if (this->bitmap == nullptr) {
    v8utils::throwError(isolate, "Error deserializing roaring bitmap");
    return scope.Escape(v8::Local<v8::Value>());
  }

  v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(this->isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;

  if (!resultMaybe.ToLocal(&result)) {
    v8utils::throwError(isolate, "Error instantiating roaring bitmap");
    return scope.Escape(v8::Local<v8::Value>());
  }

  RoaringBitmap32 * unwrapped = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (unwrapped == nullptr) {
    return scope.Escape(v8::Local<v8::Value>());
  }

  unwrapped->replaceBitmapInstance(this->isolate, this->bitmap);
  this->bitmap = nullptr;

  return scope.Escape(result);
}

///// Serialization /////

void RoaringBitmap32::getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return info.GetReturnValue().Set(0U);
  }

  SerializationFormat format =
    info.Length() > 0 ? tryParseSerializationFormat(info[0], isolate) : SerializationFormat::INVALID;

  if (format == SerializationFormat::INVALID) {
    return v8utils::throwError(
      info.GetIsolate(), "RoaringBitmap32::getSerializationSizeInBytes format argument was invalid");
  }

  switch (format) {
    case SerializationFormat::croaring: {
      auto cardinality = roaring_bitmap_get_cardinality(self->roaring);
      auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);
      auto portablesize = roaring_bitmap_portable_size_in_bytes(self->roaring);
      if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
        return info.GetReturnValue().Set((double)(portablesize) + 1);
      }
      return info.GetReturnValue().Set((double)(sizeasarray) + 1);
    }

    case SerializationFormat::portable: {
      return info.GetReturnValue().Set((double)(roaring_bitmap_portable_size_in_bytes(self->roaring)));
    }

    case SerializationFormat::unsafe_frozen_croaring: {
      return info.GetReturnValue().Set((double)(roaring_bitmap_frozen_size_in_bytes(self->roaring)));
    }

    default: {
      return v8utils::throwError(
        info.GetIsolate(), "RoaringBitmap32::getSerializationSizeInBytes format argument was invalid");
    }
  }
}

class RoaringBitmapDeserializer final {
 public:
  DeserializationFormat format = DeserializationFormat::INVALID;
  v8::Isolate * isolate = nullptr;

  RoaringBitmap32 * targetBitmap = nullptr;
  v8utils::TypedArrayContent<uint8_t> inputBuffer;

  roaring_bitmap_t_ptr volatile roaring = nullptr;
  uint8_t * volatile frozenBuffer = nullptr;

  ~RoaringBitmapDeserializer() {
    if (this->frozenBuffer != nullptr) {
      roaring_bitmap_aligned_free(this->frozenBuffer);
    }
    if (this->roaring) {
      roaring_bitmap_free(this->roaring);
    }
  }

  const char * setOutput(v8::Isolate * isolate, const v8::MaybeLocal<v8::Value> & valueMaybe, DeserializationFormat format) {
    this->isolate = isolate;
    this->format = format;

    if (valueMaybe.IsEmpty()) {
      return nullptr;
    }

    v8::Local<v8::Value> v;
    if (!valueMaybe.ToLocal(&v) || v->IsNullOrUndefined()) {
      return nullptr;
    }

    if (!this->inputBuffer.set(isolate, v)) {
      return "RoaringBitmap32 deserialization output argument was not a valid typed array";
    }

    return nullptr;
  }

  const char * parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info, bool isInstanceMethod) {
    v8::Isolate * isolate = info.GetIsolate();
    this->isolate = isolate;
    v8::HandleScope scope(isolate);

    if (isInstanceMethod) {
      this->targetBitmap = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
      if (this->targetBitmap == nullptr) {
        return "RoaringBitmap32 deserialization on invalid object";
      }
      if (this->targetBitmap->isFrozen()) {
        return ERROR_FROZEN;
      }
    }

    if (info.Length() < 2) {
      return "RoaringBitmap32 deserialization expects a format and a buffer arguments";
    }

    int bufferArgIndex = 1;
    DeserializationFormat fmt = tryParseDeserializationFormat(info[0], isolate);
    if (fmt == DeserializationFormat::INVALID) {
      bufferArgIndex = 0;
      fmt = tryParseDeserializationFormat(info[1], isolate);
    }
    this->format = fmt;

    if (
      !info[bufferArgIndex]->IsNullOrUndefined() &&
      !this->inputBuffer.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
      return "RoaringBitmap32 deserialization buffer argument was invalid";
    }

    return nullptr;
  }

  const char * deserialize() {
    if (this->format == DeserializationFormat::INVALID) {
      return "RoaringBitmap32 deserialization format argument was invalid";
    }

    auto bufLen = this->inputBuffer.length;
    const char * bufaschar = (const char *)this->inputBuffer.data;

    if (bufLen == 0 || !bufaschar) {
      // Empty bitmap for an empty buffer.
      this->roaring = roaring_bitmap_create();
      return this->roaring ? nullptr : "RoaringBitmap32 deserialization failed to create an empty bitmap";
    }

    switch (this->format) {
      case DeserializationFormat::portable: {
        this->roaring = roaring_bitmap_portable_deserialize_safe(bufaschar, bufLen);
        return this->roaring ? nullptr : "RoaringBitmap32::deserialize - portable deserialization failed";
      }

      case DeserializationFormat::croaring: {
        switch ((unsigned char)bufaschar[0]) {
          case CROARING_SERIALIZATION_ARRAY_UINT32: {
            uint32_t card;
            memcpy(&card, bufaschar + 1, sizeof(uint32_t));

            if (card * sizeof(uint32_t) + sizeof(uint32_t) + 1 != bufLen) {
              return "RoaringBitmap32 deserialization corrupted data, wrong cardinality header";
            }

            const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
            this->roaring = roaring_bitmap_of_ptr(card, elems);
            return this->roaring ? nullptr : "RoaringBitmap32 deserialization - uint32 array deserialization failed";
          }

          case CROARING_SERIALIZATION_CONTAINER: {
            this->roaring = roaring_bitmap_portable_deserialize_safe(bufaschar + 1, bufLen - 1);
            return this->roaring ? nullptr : "RoaringBitmap32 deserialization - container deserialization failed";
          }
        }

        return "RoaringBitmap32 deserialization - invalid portable header byte";
      }

      case DeserializationFormat::unsafe_frozen_portable:
      case DeserializationFormat::unsafe_frozen_croaring: {
        this->frozenBuffer = (uint8_t *)roaring_bitmap_aligned_malloc(32, bufLen);
        if (!this->frozenBuffer) {
          return "RoaringBitmap32 deserialization - failed to allocate memory for frozen bitmap";
        }
        memcpy(this->frozenBuffer, bufaschar, bufLen);

        if (format == DeserializationFormat::unsafe_frozen_croaring) {
          this->roaring =
            const_cast<roaring_bitmap_t_ptr>(roaring_bitmap_frozen_view((const char *)this->frozenBuffer, bufLen));
          return this->roaring ? nullptr : "RoaringBitmap32 deserialization - failed to create a frozen view";
        }

        this->roaring =
          const_cast<roaring_bitmap_t_ptr>(roaring_bitmap_portable_deserialize_frozen((const char *)this->frozenBuffer));
        return this->roaring ? nullptr : "RoaringBitmap32 deserialization - failed to create a frozen view";
      }

      default: return "RoaringBitmap32::deserialize - unknown deserialization format";
    }
  }

  void finalizeTargetBitmap(RoaringBitmap32 * targetBitmap) {
    targetBitmap->replaceBitmapInstance(this->isolate, this->roaring);
    this->roaring = nullptr;

    if (this->frozenBuffer) {
      targetBitmap->frozenCounter = RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN;
      targetBitmap->frozenStorage.data = this->frozenBuffer;
      targetBitmap->frozenStorage.length = std::numeric_limits<size_t>::max();
      this->frozenBuffer = nullptr;
    }
  }
};

class RoaringBitmapSerializer final {
 public:
  RoaringBitmap32 * self = nullptr;
  SerializationFormat format = SerializationFormat::INVALID;
  v8utils::TypedArrayContent<uint8_t> inputBuffer;
  size_t offset = 0;

  size_t volatile serializedSize = 0;
  uint8_t * volatile allocatedBuffer = nullptr;

  const char * parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Isolate * isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);
    int formatArgIndex = 0;
    int bufferArgIndex = -1;
    int offsetArgIndex = -1;

    this->self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
    if (this->self == nullptr) {
      return "RoaringBitmap32 serialization on invalid object";
    }
    if (info.Length() <= 0) {
      return "RoaringBitmap32 serialization format argument was not provided";
    }
    if (info.Length() > 1) {
      if (info[0]->IsUint8Array() || info[0]->IsInt8Array() || info[0]->IsUint8ClampedArray() || info[0]->IsArrayBuffer()) {
        bufferArgIndex = 0;
        formatArgIndex = 1;
        if (info.Length() > 2) {
          if (info[1]->IsNumber()) {
            offsetArgIndex = 1;
            formatArgIndex = 2;
          } else if (!info[2]->IsNullOrUndefined()) {
            return "RoaringBitmap32 serialization offset argument must be a number";
          }
        }
      } else if (
        info[1]->IsUint8Array() || info[1]->IsInt8Array() || info[1]->IsUint8ClampedArray() || info[1]->IsArrayBuffer()) {
        bufferArgIndex = 1;
        if (info.Length() > 2) {
          if (info[2]->IsNumber()) {
            offsetArgIndex = 2;
          } else if (!info[2]->IsNullOrUndefined()) {
            return "RoaringBitmap32 serialization offset argument must be a number";
          }
        }
      } else if (!info[1]->IsUndefined()) {
        return "RoaringBitmap32 serialization buffer argument was invalid";
      }
    }
    this->format = tryParseSerializationFormat(info[formatArgIndex], isolate);
    if (this->format == SerializationFormat::INVALID) {
      return "RoaringBitmap32 serialization format argument was invalid";
    }
    if (bufferArgIndex >= 0) {
      if (!this->inputBuffer.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
        return "RoaringBitmap32 serialization buffer argument was invalid";
      }
      if (offsetArgIndex >= 0) {
        double doubleOffset = 0;
        if (!info[offsetArgIndex]->NumberValue(isolate->GetCurrentContext()).To(&doubleOffset)) {
          return "RoaringBitmap32 serialization offset argument must be a number";
        }
        if (doubleOffset < 0 || !std::isfinite(doubleOffset) || doubleOffset > 9007199254740991) {
          return "RoaringBitmap32 serialization offset argument must be a positive safe integer";
        }
        this->offset = (size_t)doubleOffset;
      }
    }
    return nullptr;
  }

  const char * serialize() {
    size_t buffersize;
    size_t cardinality;
    bool serializeArray = 0;
    switch (this->format) {
      case SerializationFormat::croaring: {
        cardinality = roaring_bitmap_get_cardinality(this->self->roaring);
        auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);
        auto portablesize = roaring_bitmap_portable_size_in_bytes(this->self->roaring);
        if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
          buffersize = portablesize + 1;
        } else {
          serializeArray = true;
          buffersize = (size_t)sizeasarray + 1;
        }
        break;
      }

      case SerializationFormat::portable: {
        buffersize = roaring_bitmap_portable_size_in_bytes(this->self->roaring);
        break;
      }

      case SerializationFormat::unsafe_frozen_croaring: {
        buffersize = roaring_bitmap_frozen_size_in_bytes(this->self->roaring);
        break;
      }

      default: return "RoaringBitmap32 serialization format is invalid";
    }

    this->serializedSize = buffersize;
    uint8_t * data = this->inputBuffer.data;

    if (data == nullptr) {
      data =
        (uint8_t *)bare_aligned_malloc(this->format == SerializationFormat::unsafe_frozen_croaring ? 32 : 8, buffersize);
      if (!data) {
        return "RoaringBitmap32 serialization allocation failed";
      }
      this->allocatedBuffer = data;
    } else if (this->inputBuffer.length < this->offset + buffersize) {
      return "RoaringBitmap32 serialization buffer is too small";
    } else if (this->offset + buffersize > this->inputBuffer.length) {
      return "RoaringBitmap32::serialize offset argument plus the bitmap size is greater than the buffer size";
    }

    data += this->offset;

    switch (format) {
      case SerializationFormat::croaring: {
        if (serializeArray) {
          data[0] = CROARING_SERIALIZATION_ARRAY_UINT32;
          memcpy(data + 1, &cardinality, sizeof(uint32_t));
          roaring_bitmap_to_uint32_array(self->roaring, (uint32_t *)(data + 1 + sizeof(uint32_t)));
        } else {
          data[0] = CROARING_SERIALIZATION_CONTAINER;
          roaring_bitmap_portable_serialize(self->roaring, (char *)data + 1);
        }
        break;
      }

      case SerializationFormat::portable: {
        roaring_bitmap_portable_serialize(self->roaring, (char *)data);
        break;
      }

      case SerializationFormat::unsafe_frozen_croaring: {
        roaring_bitmap_frozen_serialize(self->roaring, (char *)data);
        break;
      }

      default: return "RoaringBitmap32 serialization format is invalid";
    }
    return nullptr;
  }

  ~RoaringBitmapSerializer() {
    if (this->allocatedBuffer) {
      bare_aligned_free(this->allocatedBuffer);
    }
  }
};

class SerializeWorker final : public v8utils::AsyncWorker {
 public:
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bitmapPersistent;
  RoaringBitmapSerializer serializer;

  explicit SerializeWorker(v8::Isolate * isolate) : v8utils::AsyncWorker(isolate) {
    gcaware_adjustAllocatedMemory(sizeof(SerializeWorker));
  }

  ~SerializeWorker() { gcaware_adjustAllocatedMemory(-sizeof(SerializeWorker)); }

  void parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    const char * error = this->serializer.parseArguments(info);
    if (error) {
      this->setError(error);
    }
    this->bitmapPersistent.Reset(isolate, info.Holder());
  }

 protected:
  // Called before the thread starts, in the main thread.
  void before() final {
    if (this->serializer.self) {
      this->serializer.self->beginFreeze();
    }
  }

  void work() final {
    const char * error = this->serializer.serialize();
    if (error != nullptr) {
      this->setError(error);
    }
  }

  void finally() final {
    if (this->serializer.self) {
      this->serializer.self->endFreeze();
    }
  }

  v8::Local<v8::Value> done() final {
    v8::EscapableHandleScope scope(isolate);
    uint8_t * allocatedBuffer = this->serializer.allocatedBuffer;

    if (allocatedBuffer) {
      // Create a new buffer using the allocated memory
      v8::MaybeLocal<v8::Object> nodeBufferMaybeLocal = node::Buffer::New(
        isolate, (char *)allocatedBuffer, serializer.serializedSize, buffer_aligned_free_callback, nullptr);

      v8::Local<v8::Object> nodeBuffer;
      if (!nodeBufferMaybeLocal.ToLocal(&nodeBuffer)) {
        this->setError("RoaringBitmap32 serialization failed to create a new buffer");
        return scope.Escape(v8::Local<v8::Value>());
      }

      this->serializer.allocatedBuffer = nullptr;
      return scope.Escape(nodeBuffer);
    }

    return scope.Escape(this->serializer.inputBuffer.bufferPersistent.Get(isolate));
  }
};

void RoaringBitmap32::serialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmapSerializer serializer;
  const char * error = serializer.parseArguments(info);
  if (error == nullptr) {
    error = serializer.serialize();
  }
  if (error != nullptr) {
    return v8utils::throwError(isolate, error);
  }
  if (serializer.allocatedBuffer) {
    // Create a new buffer using the allocated memory

    v8::MaybeLocal<v8::Object> nodeBufferMaybeLocal = node::Buffer::New(
      isolate, (char *)serializer.allocatedBuffer, serializer.serializedSize, buffer_aligned_free_callback, nullptr);

    v8::Local<v8::Object> nodeBuffer;
    if (!nodeBufferMaybeLocal.ToLocal(&nodeBuffer)) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization failed to create a new buffer");
    }

    serializer.allocatedBuffer = nullptr;

    info.GetReturnValue().Set(nodeBuffer);
  } else {
    info.GetReturnValue().Set(serializer.inputBuffer.bufferPersistent.Get(isolate));
  }
}

void RoaringBitmap32::serializeAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  SerializeWorker * worker = new SerializeWorker(isolate);
  if (!worker) {
    return v8utils::throwError(isolate, "RoaringBitmap32 serialization allocation failed");
  }
  worker->parseArguments(info);
  info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
}

void RoaringBitmap32::unsafeFrozenViewStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return;
  }

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  self->replaceBitmapInstance(isolate, nullptr);

  if (info.Length() < 2) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView expects a format and a buffer arguments");
  }

  int bufferArgIndex = 1;
  FrozenViewFormat format = tryParseFrozenViewFormat(info[0], isolate);
  if (format == FrozenViewFormat::INVALID) {
    bufferArgIndex = 0;
    format = tryParseFrozenViewFormat(info[1], isolate);
  }
  if (format == FrozenViewFormat::INVALID) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView format argument is invalid");
  }

  v8utils::TypedArrayContent<uint8_t> storage(isolate, info[bufferArgIndex]);

  v8utils::TypedArrayContent<uint8_t> & frozenStorage = self->frozenStorage;

  if (
    !info[bufferArgIndex]->IsNullOrUndefined() &&
    !frozenStorage.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView buffer argument was invalid");
  }

  self->frozenCounter = FROZEN_COUNTER_HARD_FROZEN;

  roaring_bitmap_t * bitmap = nullptr;

  switch (format) {
    case FrozenViewFormat::unsafe_frozen_croaring: {
      if (!is_pointer_aligned(storage.data, 32)) {
        return v8utils::throwError(
          isolate,
          "RoaringBitmap32::unsafeFrozenView with format unsafe_frozen_croaring requires the buffer to be 32 bytes aligned. "
          "You can use bufferAlignedAlloc, bufferAlignedAllocUnsafe or ensureBufferAligned exposed by the roaring library.");
      }

      bitmap =
        const_cast<roaring_bitmap_t *>(roaring_bitmap_frozen_view((const char *)frozenStorage.data, frozenStorage.length));
      break;
    }
    case FrozenViewFormat::unsafe_frozen_portable: {
      bitmap = roaring_bitmap_portable_deserialize_frozen((const char *)frozenStorage.data);
      break;
    }
    default: {
      return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView format argument is invalid");
    }
  }

  if (!bitmap) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView failed to deserialize the input");
  }

  self->replaceBitmapInstance(isolate, bitmap);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return;
  }

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  self->replaceBitmapInstance(isolate, nullptr);

  RoaringBitmapDeserializer deserializer;
  const char * error = deserializer.parseArguments(info, false);
  if (!error) {
    error = deserializer.deserialize();
  }
  if (error) {
    return v8utils::throwError(isolate, error);
  }

  deserializer.finalizeTargetBitmap(self);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::deserialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmapDeserializer deserializer;
  const char * error = deserializer.parseArguments(info, true);
  if (!error) {
    error = deserializer.deserialize();
  }
  if (error) {
    return v8utils::throwError(isolate, error);
  }

  deserializer.targetBitmap->replaceBitmapInstance(isolate, nullptr);

  deserializer.finalizeTargetBitmap(deserializer.targetBitmap);

  info.GetReturnValue().Set(info.Holder());
}

class DeserializeWorker final : public v8utils::AsyncWorker {
 public:
  RoaringBitmapDeserializer deserializer;

  explicit DeserializeWorker(v8::Isolate * isolate) : v8utils::AsyncWorker(isolate) {
    gcaware_adjustAllocatedMemory(sizeof(DeserializeWorker));
  }

  virtual ~DeserializeWorker() { gcaware_adjustAllocatedMemory(-sizeof(DeserializeWorker)); }

  void parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    const char * error = this->deserializer.parseArguments(info, false);
    if (error) {
      this->setError(error);
    }
  }

 protected:
  void work() final {
    const char * error = this->deserializer.deserialize();
    if (error) {
      this->setError(error);
    }
  }

  v8::Local<v8::Value> done() final {
    v8::Isolate * isolate = this->isolate;
    v8::EscapableHandleScope scope(isolate);

    v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

    v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    v8::Local<v8::Object> result;
    if (!resultMaybe.ToLocal(&result)) {
      return scope.Escape(v8::Local<v8::Value>());
    }

    RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
    if (self == nullptr) {
      this->setError(ERROR_INVALID_OBJECT);
      return scope.Escape(v8::Local<v8::Value>());
    }
    self->replaceBitmapInstance(isolate, nullptr);

    this->deserializer.finalizeTargetBitmap(self);

    return scope.Escape(result);
  }
};

void RoaringBitmap32::deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto * worker = new DeserializeWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32 deserialization failed to allocate async worker");
  }

  if (info.Length() >= 3 && info[2]->IsFunction()) {
    worker->setCallback(info[2]);
  }

  worker->parseArguments(info);

  v8::Local<v8::Value> returnValue = v8utils::AsyncWorker::run(worker);
  info.GetReturnValue().Set(returnValue);
}

class DeserializeParallelWorker : public v8utils::ParallelAsyncWorker {
 public:
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> bufferPersistent;

  RoaringBitmapDeserializer * items;

  explicit DeserializeParallelWorker(v8::Isolate * isolate) : v8utils::ParallelAsyncWorker(isolate), items(nullptr) {}

  virtual ~DeserializeParallelWorker() {
    if (items) {
      delete[] items;
    }
  }

 protected:
  virtual void parallelWork(uint32_t index) {
    RoaringBitmapDeserializer & item = items[index];
    const char * error = item.deserialize();
    if (error != nullptr) {
      this->setError(error);
    }
  }

  v8::Local<v8::Value> done() final {
    v8::EscapableHandleScope scope(this->isolate);
    v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

    const uint32_t itemsCount = this->loopCount;
    RoaringBitmapDeserializer * items = this->items;

    v8::MaybeLocal<v8::Array> resultArrayMaybe = v8::Array::New(isolate, itemsCount);
    v8::Local<v8::Array> resultArray;
    if (!resultArrayMaybe.ToLocal(&resultArray)) {
      return scope.Escape(v8::Local<v8::Value>());
    }

    v8::Local<v8::Context> currentContext = isolate->GetCurrentContext();

    for (uint32_t i = 0; i != itemsCount; ++i) {
      v8::MaybeLocal<v8::Object> instanceMaybe = cons->NewInstance(currentContext, 0, nullptr);
      v8::Local<v8::Object> instance;
      if (!instanceMaybe.ToLocal(&instance)) {
        return scope.Escape(v8::Local<v8::Value>());
      }

      RoaringBitmap32 * unwrapped = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(instance, isolate);
      if (unwrapped == nullptr) {
        this->setError(ERROR_INVALID_OBJECT);
        return scope.Escape(v8::Local<v8::Value>());
      }

      RoaringBitmapDeserializer & item = items[i];
      item.finalizeTargetBitmap(unwrapped);
      v8utils::ignoreMaybeResult(resultArray->Set(currentContext, i, instance));
    }

    return scope.Escape(resultArray);
  }
};

void RoaringBitmap32::deserializeParallelStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto * worker = new DeserializeParallelWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "Failed to allocate async worker");
  }

  if (info.Length() >= 3 && info[2]->IsFunction()) {
    worker->setCallback(info[2]);
  }

  if (info.Length() < 2) {
    worker->setError("RoaringBitmap32::deserializeAsync - requires at least two arguments");
    return info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
  }

  if (!info[0]->IsArray()) {
    worker->setError("RoaringBitmap32::deserializeParallelAsync requires an array as first argument");
    return info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
  }

  auto array = v8::Local<v8::Array>::Cast(info[0]);
  uint32_t length = array->Length();

  if (length > 0x01FFFFFF) {
    worker->setError("RoaringBitmap32::deserializeParallelAsync - array too big");
    return info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
  }

  DeserializationFormat format = tryParseDeserializationFormat(info[1], isolate);
  if (format == DeserializationFormat::INVALID) {
    worker->setError("RoaringBitmap32::deserializeAsync - second argument must be a valid deserialization format");
    return info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
  }

  RoaringBitmapDeserializer * items = length ? new RoaringBitmapDeserializer[length]() : nullptr;
  if (items == nullptr && length != 0) {
    worker->setError("RoaringBitmap32::deserializeParallelAsync - failed to allocate array of deserializers");
    return info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
  }

  worker->items = items;
  worker->loopCount = length;

  auto context = isolate->GetCurrentContext();
  for (uint32_t i = 0; i != length; ++i) {
    const char * err = items[i].setOutput(isolate, array->Get(context, i), format);
    if (err != nullptr) {
      worker->setError(err);
      return info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
    }
  }

  info.GetReturnValue().Set(v8utils::AsyncWorker::run(worker));
}

void RoaringBitmap32::isSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self == other || (self && other && roaring_bitmap_is_subset(self->roaring, other->roaring)));
}

void RoaringBitmap32::isStrictSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other && roaring_bitmap_is_strict_subset(self->roaring, other->roaring));
}

void RoaringBitmap32::isEqual(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(
    self == other || (self && other && roaring_bitmap_equals(self->roaring, other->roaring) ? true : false));
}

void RoaringBitmap32::intersects(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other && roaring_bitmap_intersect(self->roaring, other->roaring) ? true : false);
}

void RoaringBitmap32::andCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_and_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::orCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_or_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::andNotCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_andnot_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::xorCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (self == nullptr) {
    return info.GetReturnValue().Set(0);
  }
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_xor_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::jaccardIndex(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? roaring_bitmap_jaccard_index(self->roaring, other->roaring) : -1);
}

inline bool roaringAddMany(v8::Isolate * isolate, RoaringBitmap32 * self, v8::Local<v8::Value> arg, bool replace = false) {
  if (self->isFrozen()) {
    v8utils::throwError(isolate, ERROR_FROZEN);
    return false;
  }

  if (arg.IsEmpty()) {
    return false;
  }

  if (arg->IsNullOrUndefined()) {
    if (replace && self->roaring->high_low_container.containers != nullptr) {
      roaring_bitmap_clear(self->roaring);
    }
    return true;
  }

  if (!arg->IsObject()) {
    return false;
  }

  if (arg->IsUint32Array() || arg->IsInt32Array()) {
    if (replace && self->roaring->high_low_container.containers != nullptr) {
      roaring_bitmap_clear(self->roaring);
    }
    const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, arg);
    roaring_bitmap_add_many(self->roaring, typedArray.length, typedArray.data);
    self->invalidate();
    return true;
  }

  RoaringBitmap32 * other =
    v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
  if (other != nullptr) {
    if (self != other) {
      if (replace || self->roaring->high_low_container.containers == nullptr) {
        if (self->roaring->high_low_container.containers != nullptr) {
          roaring_bitmap_clear(self->roaring);
        }
        roaring_bitmap_t * copied = roaring_bitmap_copy(other->roaring);
        if (copied == nullptr) {
          v8utils::throwError(isolate, "RoaringBitmap32 - Failed to copy bitmap");
        }
        self->replaceBitmapInstance(isolate, copied);
      } else {
        roaring_bitmap_or_inplace(self->roaring, other->roaring);
      }
      self->invalidate();
    }
    return true;
  }

  v8::Local<v8::Value> argv[] = {arg};
  auto tMaybe =
    JSTypes::Uint32Array_from.Get(isolate)->Call(isolate->GetCurrentContext(), JSTypes::Uint32Array.Get(isolate), 1, argv);
  v8::Local<v8::Value> t;
  if (!tMaybe.ToLocal(&t)) return false;

  const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, t);
  if (replace) {
    roaring_bitmap_clear(self->roaring);
  }
  roaring_bitmap_add_many(self->roaring, typedArray.length, typedArray.data);
  self->invalidate();
  return true;
}

void RoaringBitmap32::copyFrom(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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

void RoaringBitmap32::addMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
  auto isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (info.Length() > 0) {
    self->invalidate();
    if (roaringAddMany(isolate, self, info[0])) {
      return info.GetReturnValue().Set(info.Holder());
    }
  }
  v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32::add(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint32_t v;
  auto isolate = info.GetIsolate();

  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }

  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::add - 32 bit unsigned integer expected");
  }

  roaring_bitmap_add(self->roaring, v);
  self->invalidate();
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::tryAdd(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }

  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(false);
  }

  bool result = roaring_bitmap_add_checked(self->roaring, v);
  if (result) {
    self->invalidate();
  }
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::removeMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
  bool done = false;
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }

  if (info.Length() > 0) {
    auto const & arg = info[0];

    if (arg->IsUint32Array() || arg->IsInt32Array()) {
      const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, arg);
      roaring_bitmap_remove_many(self->roaring, typedArray.length, typedArray.data);
      self->invalidate();
      done = true;
    } else {
      RoaringBitmap32 * other =
        v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
      if (other != nullptr) {
        roaring_bitmap_andnot_inplace(self->roaring, other->roaring);
        self->invalidate();
        done = true;
      } else {
        v8::Local<v8::Value> argv[] = {arg};
        auto tMaybe = JSTypes::Uint32Array_from.Get(isolate)->Call(
          isolate->GetCurrentContext(), JSTypes::Uint32Array.Get(isolate), 1, argv);
        v8::Local<v8::Value> t;
        if (tMaybe.ToLocal(&t)) {
          const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, t);
          roaring_bitmap_remove_many(self->roaring, typedArray.length, typedArray.data);
          self->invalidate();
          done = true;
        } else {
          RoaringBitmap32 tmp(0);
          if (roaringAddMany(isolate, &tmp, arg)) {
            roaring_bitmap_andnot_inplace(self->roaring, tmp.roaring);
            self->invalidate();
            done = true;
          }
        }
      }
    }
  }

  if (done) {
    info.GetReturnValue().Set(info.Holder());
  } else {
    v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
  }
}

void RoaringBitmap32::andInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * other =
      v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
    if (other != nullptr) {
      roaring_bitmap_and_inplace(self->roaring, other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    } else {
      RoaringBitmap32 tmp(0);
      if (roaringAddMany(isolate, &tmp, arg)) {
        roaring_bitmap_and_inplace(self->roaring, tmp.roaring);
        self->invalidate();
        return info.GetReturnValue().Set(info.Holder());
      }
    }
  }

  return v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32::xorInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * other =
      v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
    if (other != nullptr) {
      roaring_bitmap_xor_inplace(self->roaring, other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    } else {
      RoaringBitmap32 tmp(0);
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_xor_inplace(self->roaring, tmp.roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    }
  }

  return v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32::remove(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  uint32_t v;
  if (info.Length() >= 1 && info[0]->IsUint32() && info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    roaring_bitmap_remove(self->roaring, v);
    self->invalidate();
  }
}

void RoaringBitmap32::removeChecked(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(false);
  }
  bool result = roaring_bitmap_remove_checked(self->roaring, v);
  if (result) {
    self->invalidate();
  }
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::clear(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    info.GetReturnValue().Set(false);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (self->roaring && self->roaring->high_low_container.size == 0) {
    info.GetReturnValue().Set(false);
  } else {
    if (self->roaring != nullptr) {
      roaring_bitmap_clear(self->roaring);
      roaring_bitmap_shrink_to_fit(self->roaring);
      self->invalidate();
    }
    info.GetReturnValue().Set(true);
  }
}

inline static bool getRangeOperationParameters(
  const v8::FunctionCallbackInfo<v8::Value> & info, uint64_t & minInteger, uint64_t & maxInteger) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return false;
  }

  v8::Isolate * isolate = info.GetIsolate();
  double minimum, maximum;

  if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&minimum) || std::isnan(minimum)) {
    return false;
  }

  if (!info[1]->NumberValue(isolate->GetCurrentContext()).To(&maximum) || std::isnan(maximum)) {
    return false;
  }

  if (minimum < 0) {
    minimum = 0;
  }

  if (maximum < 0) {
    maximum = 0;
  } else if (maximum > 4294967296) {
    maximum = 4294967296;
  }

  minInteger = (uint64_t)minimum;
  maxInteger = (uint64_t)maximum;

  return minInteger < maxInteger;
}

void RoaringBitmap32::rangeCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
    if (!self) {
      return info.GetReturnValue().Set(0u);
    }
    auto card = roaring_bitmap_range_cardinality(self->roaring, minInteger, maxInteger);
    if (card <= 0xFFFFFFFF) {
      return info.GetReturnValue().Set((uint32_t)card);
    }
    return info.GetReturnValue().Set((double)card);
  }
  return info.GetReturnValue().Set(0u);
}

void RoaringBitmap32::flipRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(info.GetIsolate(), ERROR_FROZEN);
  }
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    if (self != nullptr) {
      roaring_bitmap_flip_inplace(self->roaring, minInteger, maxInteger);
      self->invalidate();
    }
  }
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::addRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(info.GetIsolate(), ERROR_FROZEN);
  }
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    if (self != nullptr) {
      roaring_bitmap_add_range_closed(self->roaring, (uint32_t)minInteger, (uint32_t)(maxInteger - 1));
      self->invalidate();
    }
  }
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::removeRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  RoaringBitmap32 * self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(info.GetIsolate(), ERROR_FROZEN);
  }
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    if (self != nullptr) {
      roaring_bitmap_remove_range_closed(self->roaring, (uint32_t)minInteger, (uint32_t)(maxInteger - 1));
      self->invalidate();
    }
  }
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::swapStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::swap second argument must be a RoaringBitmap32");

  if (a->isFrozen() || b->isFrozen()) return v8utils::throwError(isolate, ERROR_FROZEN);

  if (a != b) {
    auto * a_roaring = a->roaring;
    a->roaring = b->roaring;
    b->roaring = a_roaring;

    a->invalidate();
    b->invalidate();
  }
}

void RoaringBitmap32::addOffsetStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset expects 2 arguments");

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset first argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset failed to create new instance");
  }

  auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  // Second argument is the offset, read as double
  double offset = info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(NAN);
  if (std::isnan(offset)) {
    offset = 0;
  } else if (offset < -4294967296) {
    offset = -4294967296;
  } else if (offset > 4294967296) {
    offset = 4294967296;
  }

  roaring_bitmap_t * r = roaring_bitmap_add_offset(a->roaring, (int64_t)offset);
  if (r == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset failed materalization");
  }
  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::andStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::and expects 2 arguments");

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::and first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::and second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::and failed to create new instance");
  }

  auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_and(a->roaring, b->roaring);
  if (r == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::and failed materalization");
  }

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::orStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_or(a->roaring, b->roaring);
  if (r == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or failed materalization");

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::xorStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_xor(a->roaring, b->roaring);
  if (r == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor failed materalization");

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::andNotStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_andnot(a->roaring, b->roaring);
  if (r == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot failed materalization");
  }

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::fromRangeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  uint32_t v;
  uint32_t step = 1;
  if (info.Length() >= 3 && info[2]->IsUint32() && info[2]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    step = v;
    if (step == 0) {
      step = 1;
    }
  }

  auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    roaring_bitmap_t * r = roaring_bitmap_from_range(minInteger, maxInteger, step);
    if (r != nullptr) {
      self->replaceBitmapInstance(isolate, r);
    }
  }

  info.GetReturnValue().Set(result);
}

template <typename TSize>
void RoaringBitmap32_opManyStatic(
  const char * opName,
  roaring_bitmap_t * op(TSize number, const roaring_bitmap_t ** x),
  const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  int length = info.Length();

  v8::Local<v8::FunctionTemplate> ctorType = RoaringBitmap32::constructorTemplate.Get(isolate);
  v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

  auto context = isolate->GetCurrentContext();

  if (length == 0) {
    auto vMaybe = cons->NewInstance(context, 0, nullptr);
    v8::Local<v8::Object> v;
    if (vMaybe.ToLocal(&v)) {
      info.GetReturnValue().Set(v);
    }
    return;
  }

  if (length == 1) {
    if (info[0]->IsArray()) {
      auto array = v8::Local<v8::Array>::Cast(info[0]);

      size_t arrayLength = array->Length();

      if (arrayLength == 0) {
        auto vMaybe = cons->NewInstance(context, 0, nullptr);
        v8::Local<v8::Object> v;
        if (vMaybe.ToLocal(&v)) {
          info.GetReturnValue().Set(v);
        }
        return;
      }

      if (arrayLength == 1) {
        auto itemMaybe = array->Get(context, 0);

        v8::Local<v8::Value> item;
        if (!itemMaybe.ToLocal(&item) || !ctorType->HasInstance(item)) {
          return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
        }

        v8::Local<v8::Value> argv[] = {item};
        auto vMaybe = cons->NewInstance(context, 1, argv);
        v8::Local<v8::Object> v;
        if (vMaybe.ToLocal(&v)) {
          info.GetReturnValue().Set(v);
        }
        return;
      }

      const auto ** x = (const roaring_bitmap_t **)gcaware_malloc(arrayLength * sizeof(roaring_bitmap_t *));
      if (x == nullptr) {
        return v8utils::throwTypeError(isolate, opName, " failed allocation");
      }

      for (size_t i = 0; i < arrayLength; ++i) {
        v8::Local<v8::Value> item;
        auto itemMaybe = array->Get(context, i);
        if (!itemMaybe.ToLocal(&item) || !ctorType->HasInstance(item)) {
          return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
        }
        RoaringBitmap32 * p = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(item, ctorType, isolate);
        if (p == nullptr) {
          gcaware_free(x);
          return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
        }
        x[i] = p->roaring;
      }

      auto resultMaybe = cons->NewInstance(context, 0, nullptr);
      v8::Local<v8::Object> result;
      if (!resultMaybe.ToLocal(&result)) {
        gcaware_free(x);
        return;
      }

      auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
      if (!self) {
        return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
      }

      roaring_bitmap_t * r = op((TSize)arrayLength, x);
      if (r == nullptr) {
        gcaware_free(x);
        return v8utils::throwTypeError(isolate, opName, " failed roaring allocation");
      }

      self->replaceBitmapInstance(isolate, r);

      info.GetReturnValue().Set(result);

    } else {
      if (!ctorType->HasInstance(info[0])) {
        return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
      }

      v8::Local<v8::Value> argv[] = {info[0]};
      auto vMaybe = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
      v8::Local<v8::Object> v;
      if (vMaybe.ToLocal(&v)) {
        info.GetReturnValue().Set(v);
      }
    }
  } else {
    const auto ** x = (const roaring_bitmap_t **)gcaware_malloc(length * sizeof(roaring_bitmap_t *));
    if (x == nullptr) {
      return v8utils::throwTypeError(isolate, opName, " failed allocation");
    }

    for (int i = 0; i < length; ++i) {
      RoaringBitmap32 * p = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, i, ctorType);
      if (p == nullptr) {
        gcaware_free(x);
        return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
      }
      x[i] = p->roaring;
    }

    v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    v8::Local<v8::Object> result;
    if (!resultMaybe.ToLocal(&result)) {
      gcaware_free(x);
      return;
    }

    auto self = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
    if (!self) {
      return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
    }

    roaring_bitmap_t * r = op((TSize)length, x);
    if (r == nullptr) {
      gcaware_free(x);
      return v8utils::throwTypeError(isolate, opName, " failed roaring allocation");
    }

    self->replaceBitmapInstance(isolate, r);

    info.GetReturnValue().Set(result);
  }
}

void RoaringBitmap32::orManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32_opManyStatic("RoaringBitmap32::orMany", roaring_bitmap_or_many_heap, info);
}

void RoaringBitmap32::xorManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32_opManyStatic("RoaringBitmap32::xorMany", roaring_bitmap_xor_many, info);
}

class FromArrayAsyncWorker : public RoaringBitmap32FactoryAsyncWorker {
 public:
  v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> argPersistent;
  v8utils::TypedArrayContent<uint32_t> buffer;

  explicit FromArrayAsyncWorker(v8::Isolate * isolate) : RoaringBitmap32FactoryAsyncWorker(isolate) {
    gcaware_adjustAllocatedMemory(sizeof(FromArrayAsyncWorker));
  }

  virtual ~FromArrayAsyncWorker() { gcaware_adjustAllocatedMemory(-sizeof(FromArrayAsyncWorker)); }

 protected:
  void work() final {
    bitmap = roaring_bitmap_create_with_capacity(buffer.length);
    if (bitmap == nullptr) {
      this->setError("Failed to allocate roaring bitmap");
      return;
    }
    roaring_bitmap_add_many(bitmap, buffer.length, buffer.data);
    roaring_bitmap_run_optimize(bitmap);
    roaring_bitmap_shrink_to_fit(bitmap);
  }
};

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
      if (RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(arg)) {
        return v8utils::throwTypeError(
          isolate, "RoaringBitmap32::fromArrayAsync cannot be called with a RoaringBitmap32 instance");
      }
      v8::Local<v8::Value> argv[] = {arg};
      auto carg = JSTypes::Uint32Array_from.Get(isolate)->Call(
        isolate->GetCurrentContext(), JSTypes::Uint32Array.Get(isolate), 1, argv);
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

  v8::Local<v8::Value> returnValue = v8utils::AsyncWorker::run(worker);
  info.GetReturnValue().Set(returnValue);
}

////////////// RoaringBitmap32BufferedIterator //////////////

v8::Eternal<v8::FunctionTemplate> RoaringBitmap32BufferedIterator::constructorTemplate;
v8::Eternal<v8::Function> RoaringBitmap32BufferedIterator::constructor;
v8::Eternal<v8::String> RoaringBitmap32BufferedIterator::nPropertyName;

RoaringBitmap32BufferedIterator::RoaringBitmap32BufferedIterator() {
  this->it.parent = nullptr;
  this->it.has_value = false;
  gcaware_adjustAllocatedMemory(sizeof(RoaringBitmap32BufferedIterator));
}

void RoaringBitmap32BufferedIterator::destroy() {
  this->bitmap.Reset();
  if (!persistent.IsEmpty()) {
    persistent.ClearWeak();
    persistent.Reset();
  }
  gcaware_adjustAllocatedMemory(-sizeof(RoaringBitmap32BufferedIterator));
}

RoaringBitmap32BufferedIterator::~RoaringBitmap32BufferedIterator() { this->destroy(); }

void RoaringBitmap32BufferedIterator::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto stringN = NEW_LITERAL_V8_STRING(isolate, "n", v8::NewStringType::kInternalized);
  auto className = NEW_LITERAL_V8_STRING(isolate, "RoaringBitmap32BufferedIterator", v8::NewStringType::kInternalized);

  nPropertyName.Set(isolate, stringN);

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, New);

  ctor->SetClassName(className);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  constructorTemplate.Set(isolate, ctor);

  NODE_SET_PROTOTYPE_METHOD(ctor, "fill", fill);

  auto ctorFunctionMaybe = ctor->GetFunction(isolate->GetCurrentContext());
  v8::Local<v8::Function> ctorFunction;

  if (!ctorFunctionMaybe.ToLocal(&ctorFunction)) {
    return v8utils::throwError(isolate, "Failed to instantiate RoaringBitmap32BufferedIterator");
  }

  constructor.Set(isolate, ctorFunction);
  v8utils::defineHiddenField(isolate, exports, "RoaringBitmap32BufferedIterator", ctorFunction);
}

void RoaringBitmap32BufferedIterator::New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - needs to be called with new");
  }

  auto holder = info.Holder();

  if (info.Length() != 2) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - needs two arguments");
  }

  RoaringBitmap32 * bitmapInstance =
    v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], RoaringBitmap32::constructorTemplate, isolate);
  if (bitmapInstance == nullptr) {
    return v8utils::throwTypeError(
      isolate, "RoaringBitmap32BufferedIterator::ctor - first argument must be of type RoaringBitmap32");
  }

  auto bufferObject = info[1];

  if (!bufferObject->IsUint32Array()) {
    return v8utils::throwTypeError(
      isolate, "RoaringBitmap32BufferedIterator::ctor - second argument must be of type Uint32Array");
  }

  const v8utils::TypedArrayContent<uint32_t> bufferContent(isolate, bufferObject);
  if (!bufferContent.data || bufferContent.length < 1) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - invalid Uint32Array buffer");
  }

  auto * instance = new RoaringBitmap32BufferedIterator();
  if (instance == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
  }

  holder->SetAlignedPointerInInternalField(0, instance);
  instance->persistent.Reset(isolate, holder);
  instance->persistent.SetWeak(instance, WeakCallback, v8::WeakCallbackType::kParameter);

  auto context = isolate->GetCurrentContext();

  roaring_init_iterator(bitmapInstance->roaring, &instance->it);

  uint32_t n = roaring_read_uint32_iterator(&instance->it, bufferContent.data, bufferContent.length);
  if (n != 0) {
    instance->bitmapInstance = bitmapInstance;
    instance->bitmapVersion = bitmapInstance->version;

    auto a0Maybe = info[0]->ToObject(context);
    v8::Local<v8::Object> a0;
    if (!a0Maybe.ToLocal(&a0)) {
      return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
    }
    instance->bitmap.Reset(isolate, a0);

    auto a1Maybe = bufferObject->ToObject(context);
    v8::Local<v8::Object> a1;
    if (!a1Maybe.ToLocal(&a1)) {
      return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
    }

    instance->bufferContent.set(isolate, bufferObject);
  } else {
    instance->bitmapInstance = nullptr;
  }

  if (holder->Set(context, nPropertyName.Get(isolate), v8::Uint32::NewFromUnsigned(isolate, n)).IsNothing()) {
    return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - instantiation failed");
  }

  info.GetReturnValue().Set(holder);
}

void RoaringBitmap32BufferedIterator::fill(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32BufferedIterator * instance =
    v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32BufferedIterator>(info.Holder(), isolate);

  RoaringBitmap32 * bitmapInstance = instance ? instance->bitmapInstance : nullptr;

  if (bitmapInstance == nullptr) {
    return info.GetReturnValue().Set(0U);
  }

  if (bitmapInstance->version != instance->bitmapVersion) {
    return v8utils::throwError(isolate, "RoaringBitmap32 iterator - bitmap changed while iterating");
  }

  uint32_t n = roaring_read_uint32_iterator(&instance->it, instance->bufferContent.data, instance->bufferContent.length);
  if (n == 0) {
    instance->bitmapInstance = nullptr;
    instance->bufferContent.reset();
    instance->bitmap.Reset();
  }

  info.GetReturnValue().Set(n);
}

void RoaringBitmap32BufferedIterator::WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32BufferedIterator> const & info) {
  RoaringBitmap32BufferedIterator * p = info.GetParameter();
  if (p != nullptr) {
    delete p;
  }
}

#ifndef ROARING_NODE_ROARING_BITMAP32_RANGES_
#define ROARING_NODE_ROARING_BITMAP32_RANGES_

#include "RoaringBitmap32.h"
#include "async-workers.h"

inline static bool getRangeOperationParameters(
  const v8::FunctionCallbackInfo<v8::Value> & info, uint64_t & minInteger, uint64_t & maxInteger) {
  v8::Isolate * isolate = info.GetIsolate();
  double minimum = 0, maximum = 4294967296;

  if (info.Length() > 0 && !info[0]->IsUndefined()) {
    if (!info[0]->IsNumber()) {
      return false;
    }
    if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&minimum)) {
      minimum = 0;
    }
  }

  if (info.Length() > 1 && !info[1]->IsUndefined()) {
    if (!info[1]->IsNumber()) {
      return false;
    }
    if (!info[1]->NumberValue(isolate->GetCurrentContext()).To(&maximum)) {
      maximum = 4294967296;
    }
  }

  if (std::isnan(minimum) || std::isnan(maximum)) {
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

  return minimum < 4294967296 && minInteger < maxInteger;
}

void RoaringBitmap32::rangeCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
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
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
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
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
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
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
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

void RoaringBitmap32::intersectsWithRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  uint64_t minInteger, maxInteger;
  info.GetReturnValue().Set(
    self != nullptr && getRangeOperationParameters(info, minInteger, maxInteger) &&
    roaring_bitmap_intersect_with_range(self->roaring, (uint64_t)minInteger, (uint64_t)(maxInteger - 1)));
}

void RoaringBitmap32::toUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  size_t size = self->getSize();
  size_t maxSize = size;

  v8utils::TypedArrayContent<uint32_t> typedArrayContent;
  if (info.Length() >= 1 && !info[0]->IsUndefined()) {
    if (info[0]->IsNumber()) {
      double maxSizeDouble;
      if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&maxSizeDouble) || std::isnan(maxSizeDouble)) {
        return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - argument must be a valid integer number");
      }
      maxSize = maxSizeDouble <= 0 ? 0 : (maxSizeDouble >= size ? size : (size_t)maxSizeDouble);
    } else {
      if (!argumentIsValidUint32ArrayOutput(info[0]) || !typedArrayContent.set(isolate, info[0])) {
        return v8utils::throwError(
          isolate, "RoaringBitmap32::toUint32Array - argument must be a UInt32Array, Int32Array or ArrayBuffer");
      }

      bool arrayIsSmaller = typedArrayContent.length < size;
      if (arrayIsSmaller) {
        roaring_bitmap_range_uint32_array(self->roaring, 0, typedArrayContent.length, typedArrayContent.data);
        size = typedArrayContent.length;
      }

      v8::Local<v8::Value> result;
      if (!v8utils::v8ValueToUint32ArrayWithLimit(isolate, typedArrayContent.bufferPersistent.Get(isolate), size, result)) {
        return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - failed to create a new UInt32Array");
      }

      if (size != 0 && !arrayIsSmaller) {
        roaring_bitmap_to_uint32_array(self->roaring, typedArrayContent.data);
      }

      return info.GetReturnValue().Set(result);
    }
  }

  auto arrayBuffer = v8::ArrayBuffer::New(isolate, maxSize * sizeof(uint32_t));
  if (arrayBuffer.IsEmpty()) {
    return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - failed to allocate memory");
  }
  auto typedArray = v8::Uint32Array::New(arrayBuffer, 0, maxSize);
  if (typedArray.IsEmpty()) {
    return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - failed to allocate memory");
  }

  if (maxSize != 0) {
    if (!typedArrayContent.set(isolate, typedArray)) {
      return v8utils::throwError(isolate, "RoaringBitmap32::toUint32Array - failed to allocate memory");
    }
    if (maxSize < size) {
      roaring_bitmap_range_uint32_array(self->roaring, 0, maxSize, typedArrayContent.data);
    } else {
      roaring_bitmap_to_uint32_array(self->roaring, typedArrayContent.data);
    }
  }

  info.GetReturnValue().Set(typedArray);
}

void RoaringBitmap32::toUint32ArrayAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  ToUint32ArrayAsyncWorker * worker = new ToUint32ArrayAsyncWorker(info);
  if (!worker) {
    return v8utils::throwError(isolate, "RoaringBitmap32::toUint32ArrayAsync - allocation failed");
  }
  info.GetReturnValue().Set(AsyncWorker::run(worker));
}

void RoaringBitmap32::rangeUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  double num;

  int outputArgIndex = -1;
  int offsetArgIndex = -1;
  int limitArgIndex = -1;

  if (info[0]->IsNumber()) {
    offsetArgIndex = 0;

    if (info.Length() > 1) {
      if (info[1]->IsNumber()) {
        limitArgIndex = 1;
        if (info.Length() > 2 && !info[2]->IsUndefined()) {
          outputArgIndex = 2;
        }
      } else {
        outputArgIndex = 1;
      }
    }
  } else {
    outputArgIndex = 0;
    if (info.Length() > 1) {
      offsetArgIndex = 1;
      if (info.Length() > 2 && !info[2]->IsUndefined()) {
        limitArgIndex = 2;
      }
    }
  }

  size_t offset = 0;
  if (offsetArgIndex == -1) {
    if (outputArgIndex == -1) {
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - offset or output argument is missing");
    }
  } else {
    if (!info[offsetArgIndex]->NumberValue(isolate->GetCurrentContext()).To(&num) || std::isnan(num)) {
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - limit argument must be a valid integer");
    }
    offset = (size_t)std::min(std::max(num, 0.0), 4294967296.0);
  }

  if (limitArgIndex == -1 && outputArgIndex == -1) {
    return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - missing limit or output argument");
  }

  size_t limit = 4294967296;
  if (limitArgIndex != -1) {
    if (!info[limitArgIndex]->NumberValue(isolate->GetCurrentContext()).To(&num) || std::isnan(num)) {
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - limit argument must be a valid integer");
    }
    limit = (size_t)std::min(std::max(num, 0.0), 4294967296.0);
  }

  v8utils::TypedArrayContent<uint32_t> typedArrayContent;

  if (outputArgIndex != -1) {
    if (!argumentIsValidUint32ArrayOutput(info[outputArgIndex]) || !typedArrayContent.set(isolate, info[outputArgIndex])) {
      return v8utils::throwError(
        isolate, "RoaringBitmap32::rangeUint32Array - output argument must be a UInt32Array, Int32Array or ArrayBuffer");
    }
    limit = std::min(limit, typedArrayContent.length);
  }

  size_t size;
  auto cardinality = self->getSize();
  if (offset > cardinality) {
    size = 0;
  } else {
    size = limit < cardinality - offset ? limit : cardinality - offset;
    if (offset > cardinality) {
      size = 0;
    }
    if (limit > cardinality - offset) {
      limit = cardinality - offset;
    }
  }

  if (outputArgIndex == -1) {
    auto arrayBuffer = v8::ArrayBuffer::New(isolate, size * sizeof(uint32_t));
    if (arrayBuffer.IsEmpty()) {
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - failed to create an ArrayBuffer");
    }
    auto typedArray = v8::Uint32Array::New(arrayBuffer, 0, size);
    if (typedArray.IsEmpty()) {
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - failed to create an Uint32Array");
    }
    if (!typedArrayContent.set(isolate, typedArray)) {
      return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - failed to create an Uint32Array");
    }
  }

  if (size > 0) {
    if (offset == 0 && limit == cardinality) {
      roaring_bitmap_to_uint32_array(self->roaring, typedArrayContent.data);
    } else {
      if (!roaring_bitmap_range_uint32_array(self->roaring, offset, limit, typedArrayContent.data)) {
        return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32Array - failed to build the range");
      }
    }
  }

  v8::Local<v8::Value> result;
  if (!v8utils::v8ValueToUint32ArrayWithLimit(isolate, typedArrayContent.bufferPersistent.Get(isolate), size, result)) {
    return v8utils::throwError(isolate, "RoaringBitmap32::rangeUint32ArrayAsync - failed to create an Uint32Array range");
  }
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::toArray(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  size_t cardinality = self ? self->getSize() : 0;

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

  double maxLength = (double)cardinality;

  // Check if the first argument is an array
  if (info.Length() >= 1 && !info[0]->IsUndefined()) {
    if (info[0]->IsNumber()) {
      if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&maxLength)) {
        return v8utils::throwTypeError(isolate, "RoaringBitmap32::toArray - maxLength argument must be a valid number");
      }
      if (maxLength > cardinality) {
        maxLength = (double)cardinality;
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
          maxLength = (double)cardinality;
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
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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

  if (self != nullptr && !self->isEmpty()) {
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

void RoaringBitmap32::hasRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
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

void RoaringBitmap32::fromRangeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  uint32_t v;
  uint32_t step = 1;
  if (info.Length() >= 3 && info[2]->IsNumber() && info[2]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    step = v;
    if (step == 0) {
      step = 1;
    }
  }

  auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
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

#endif  // ROARING_NODE_ROARING_BITMAP32_RANGES_

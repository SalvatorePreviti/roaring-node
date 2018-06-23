#include <uv.h>
#include "RoaringBitmap32.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

void RoaringBitmap32::getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

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
    auto typedArray = JSTypes::bufferAllocUnsafe(info.GetIsolate(), portablesize);
    const v8utils::TypedArrayContent<uint8_t> buf(isolate, typedArray);
    if (!buf.length || !buf.data)
      return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

    roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data);
    info.GetReturnValue().Set(typedArray);
  } else {
    auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
    auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

    if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
      auto typedArray = JSTypes::bufferAllocUnsafe(isolate, portablesize + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(isolate, typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_CONTAINER;
      roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data + 1);
      info.GetReturnValue().Set(typedArray);
    } else {
      auto typedArray = JSTypes::bufferAllocUnsafe(isolate, (size_t)sizeasarray + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(isolate, typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_ARRAY_UINT32;
      memcpy(buf.data + 1, &cardinality, sizeof(uint32_t));
      roaring_bitmap_to_uint32_array(&self->roaring, (uint32_t *)(buf.data + 1 + sizeof(uint32_t)));
      info.GetReturnValue().Set(typedArray);
    }
  }
}

const char * RoaringBitmap32::doDeserialize(const v8utils::TypedArrayContent<uint8_t> & typedArray, bool portable, roaring_bitmap_t & bitmap) {
  bitmap = std::move(*((roaring_bitmap_t *)&RoaringBitmap32::roaring_bitmap_zero));

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

      if (!ra_init(&bitmap.high_low_container)) {
        return "RoaringBitmap32::deserialize - failed to initialize a new roaring container";
      }

      const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
      roaring_bitmap_add_many(&bitmap, card, elems);
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
    return v8utils::throwTypeError("RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer");

  const v8utils::TypedArrayContent<uint8_t> typedArray(isolate, info[0]);

  roaring_bitmap_t bitmap;
  const char * error = doDeserialize(typedArray, info.Length() > 1 && info[1]->IsTrue(), bitmap);
  if (error != nullptr) {
    return v8utils::throwError(error);
  }

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty()) {
    ra_clear(&bitmap.high_low_container);
    return;
  }

  auto result = resultMaybe.ToLocalChecked();

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  self->roaring = std::move(bitmap);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::deserialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return v8utils::throwTypeError("RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer");

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  const v8utils::TypedArrayContent<uint8_t> typedArray(isolate, info[0]);

  roaring_bitmap_t bitmap;
  const char * error = doDeserialize(typedArray, info.Length() > 1 && info[1]->IsTrue(), bitmap);
  if (error != nullptr) {
    return v8utils::throwError(error);
  }

  self->roaring.high_low_container = std::move(bitmap.high_low_container);

  info.GetReturnValue().Set(info.Holder());
}

struct DeserializeWorker {
  v8::Isolate * isolate;
  v8utils::TypedArrayContent<uint8_t> buffer;
  bool portable;
  bool isPromise;

  v8::Persistent<v8::Function> callback;
  v8::Persistent<v8::Promise::Resolver> resolver;

  uv_work_t request;
  roaring_bitmap_t bitmap;
  const char * error;

  DeserializeWorker(v8::Isolate * isolate, v8::Local<v8::Value> buffer, bool portable) :
      isolate(isolate),
      buffer(isolate, buffer),
      portable(portable),
      error(nullptr) {
    request.data = this;
  }

  ~DeserializeWorker() {
    callback.Reset();
    resolver.Reset();
  }

  static void work(uv_work_t * request) {
    DeserializeWorker * self = static_cast<DeserializeWorker *>(request->data);
    self->error = RoaringBitmap32::doDeserialize(self->buffer, self->portable, self->bitmap);
  }

  static void done(uv_work_t * request, int status) {
    DeserializeWorker * self = static_cast<DeserializeWorker *>(request->data);

    v8::Isolate * isolate = self->isolate;
    v8::HandleScope scope(isolate);

    const char * error = self->error;
    v8::Local<v8::Function> callback = self->callback.Get(isolate);
    v8::Local<v8::Promise::Resolver> resolver = self->resolver.Get(isolate);

    if (error) {
      delete self;
      v8::Local<v8::Value> errorObject = v8::Exception::Error(v8::String::NewFromUtf8(isolate, error));
      if (!callback.IsEmpty()) {
        v8::Local<v8::Value> argv[] = {errorObject, v8::Undefined(isolate)};
        callback->Call(isolate->GetCurrentContext()->Global(), 2, argv);
      }
      if (!resolver.IsEmpty()) {
        v8utils::ignoreMaybeResult(resolver->Reject(isolate->GetCurrentContext(), errorObject));
      }
      return;
    }

    v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

    auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    if (resultMaybe.IsEmpty()) {
      ra_clear(&self->bitmap.high_low_container);
      delete self;
      return;
    }

    auto result = resultMaybe.ToLocalChecked();

    RoaringBitmap32 * unwrapped = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

    ra_clear(&unwrapped->roaring.high_low_container);
    unwrapped->roaring = std::move(self->bitmap);

    delete self;

    if (!callback.IsEmpty()) {
      v8::Local<v8::Value> argv[] = {v8::Null(isolate), result};
      callback->Call(isolate->GetCurrentContext()->Global(), 2, argv);
    }

    if (!resolver.IsEmpty()) {
      v8utils::ignoreMaybeResult(resolver->Resolve(isolate->GetCurrentContext(), result));
    }
  }
};

void RoaringBitmap32::deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return v8utils::throwTypeError("RoaringBitmap32::deserializeAsync first argument must be Uint8Array or Buffer");

  bool portable = false;
  v8::Local<v8::Function> callback;
  v8::Local<v8::Promise::Resolver> resolver;

  if (info.Length() >= 2) {
    if (info[1]->IsFunction()) {
      callback = v8::Local<v8::Function>::Cast(info[1]);
      if (info.Length() >= 3 && info[2]->IsTrue()) {
        portable = true;
      }
    } else {
      if (info[1]->IsTrue()) {
        portable = true;
      }
      if (info.Length() >= 3 && info[2]->IsFunction()) {
        callback = v8::Local<v8::Function>::Cast(info[2]);
      }
    }
  }

  if (callback.IsEmpty()) {
    v8::MaybeLocal<v8::Promise::Resolver> resolverMaybe = v8::Promise::Resolver::New(isolate->GetCurrentContext());
    if (resolverMaybe.IsEmpty()) {
      return v8utils::throwTypeError("RoaringBitmap32::deserializeAsync - failed to create Promise");
    }
    resolver = resolverMaybe.ToLocalChecked();
  }

  DeserializeWorker * worker = new DeserializeWorker(isolate, info[0], portable);
  if (worker == nullptr) {
    return v8utils::throwTypeError("RoaringBitmap32::deserializeAsync - failed to allocate worker");
  }

  if (!callback.IsEmpty()) {
    worker->callback.Reset(isolate, callback);
  }

  if (!resolver.IsEmpty()) {
    worker->resolver.Reset(isolate, resolver);
  }

  int status = uv_queue_work(uv_default_loop(), &worker->request, DeserializeWorker::work, DeserializeWorker::done);
  if (status != 0) {
    delete worker;
    return v8utils::throwTypeError("RoaringBitmap32::deserializeAsync - failed to enqueue the async worker");
  }

  info.GetReturnValue().Set(resolver->GetPromise());
}

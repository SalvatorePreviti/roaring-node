#include "RoaringBitmap32BufferedIterator.h"
#include "../RoaringBitmap32/RoaringBitmap32.h"

////////////// RoaringBitmap32BufferedIterator //////////////

v8::Persistent<v8::FunctionTemplate> RoaringBitmap32BufferedIterator::constructorTemplate;
v8::Persistent<v8::Function> RoaringBitmap32BufferedIterator::constructor;
v8::Persistent<v8::String> RoaringBitmap32BufferedIterator::nPropertyName;

RoaringBitmap32BufferedIterator::RoaringBitmap32BufferedIterator() {
  this->it.parent = nullptr;
  this->it.has_value = false;
}

RoaringBitmap32BufferedIterator::~RoaringBitmap32BufferedIterator() {
  this->bitmap.Reset();
  this->buffer.Reset();
}

void RoaringBitmap32BufferedIterator::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  nPropertyName.Reset(isolate, v8::String::NewFromUtf8(isolate, "n"));

  auto className = v8::String::NewFromUtf8(isolate, "RoaringBitmap32BufferedIterator");

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, New);

  ctor->SetClassName(className);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  constructorTemplate.Reset(isolate, ctor);

  NODE_SET_PROTOTYPE_METHOD(ctor, "fill", fill);

  auto ctorFunction = ctor->GetFunction();

  constructor.Reset(isolate, ctorFunction);

  v8utils::defineHiddenField(isolate, exports, "RoaringBitmap32BufferedIterator", ctorFunction);
}

void RoaringBitmap32BufferedIterator::New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (!info.IsConstructCall()) {
    return v8utils::throwTypeError("RoaringBitmap32BufferedIterator::ctor - needs to be called with new");
  }

  auto holder = info.Holder();

  if (info.Length() != 2) {
    return v8utils::throwTypeError("RoaringBitmap32BufferedIterator::ctor - needs two arguments");
  }

  RoaringBitmap32 * bitmapInstance = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], RoaringBitmap32::constructorTemplate, isolate);
  if (!bitmapInstance) {
    return v8utils::throwTypeError("RoaringBitmap32BufferedIterator::ctor - first argument must be of type RoaringBitmap32");
  }

  auto bufferObject = info[1];

  if (!bufferObject->IsUint32Array()) {
    return v8utils::throwTypeError("RoaringBitmap32BufferedIterator::ctor - second argument must be of type Uint32Array");
  }

  const v8utils::TypedArrayContent<uint32_t> bufferContent(bufferObject);
  if (!bufferContent.data || bufferContent.length < 1) {
    return v8utils::throwTypeError("RoaringBitmap32BufferedIterator::ctor - invalid Uint32Array buffer");
  }

  RoaringBitmap32BufferedIterator * instance = new RoaringBitmap32BufferedIterator();
  if (!instance) {
    return v8utils::throwError("RoaringBitmap32BufferedIterator::ctor - allocation failed");
  }

  instance->Wrap(isolate, holder);

  roaring_init_iterator(&bitmapInstance->roaring, &instance->it);

  uint32_t n = roaring_read_uint32_iterator(&instance->it, bufferContent.data, bufferContent.length);
  if (n != 0) {
    instance->bitmapInstance = bitmapInstance;
    instance->bitmapVersion = bitmapInstance->version;
    instance->bitmap.Reset(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
    instance->buffer.Reset(isolate, bufferObject->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
    instance->bufferContent.set(bufferObject);
  } else {
    instance->bitmapInstance = nullptr;
  }

  holder->Set(nPropertyName.Get(isolate), v8::Uint32::NewFromUnsigned(isolate, n));

  info.GetReturnValue().Set(scope.Escape(holder));
}

void RoaringBitmap32BufferedIterator::fill(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32BufferedIterator * instance = v8utils::ObjectWrap::Unwrap<RoaringBitmap32BufferedIterator>(info.Holder());

  RoaringBitmap32 * bitmapInstance = instance->bitmapInstance;

  if (bitmapInstance == nullptr) {
    return info.GetReturnValue().Set(0U);
  }

  if (bitmapInstance->version != instance->bitmapVersion) {
    return v8utils::throwError("RoaringBitmap32 iterator - bitmap changed while iterating");
  }

  uint32_t n = roaring_read_uint32_iterator(&instance->it, instance->bufferContent.data, instance->bufferContent.length);
  if (n == 0) {
    instance->bitmapInstance = nullptr;
    instance->bufferContent.reset();
    instance->bitmap.Reset();
    instance->buffer.Reset();
  }

  info.GetReturnValue().Set(n);
}

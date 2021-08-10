#include "RoaringBitmap32BufferedIterator.h"
#include "../RoaringBitmap32/RoaringBitmap32.h"

////////////// RoaringBitmap32BufferedIterator //////////////

v8::Eternal<v8::FunctionTemplate> RoaringBitmap32BufferedIterator::constructorTemplate;
v8::Eternal<v8::Function> RoaringBitmap32BufferedIterator::constructor;
v8::Eternal<v8::String> RoaringBitmap32BufferedIterator::nPropertyName;

RoaringBitmap32BufferedIterator::RoaringBitmap32BufferedIterator() {
  this->it.parent = nullptr;
  this->it.has_value = false;
}

void RoaringBitmap32BufferedIterator::destroy() {
  this->bitmap.Reset();
  this->buffer.Reset();
  if (!persistent.IsEmpty()) {
    persistent.ClearWeak();
    persistent.Reset();
  }
}

RoaringBitmap32BufferedIterator::~RoaringBitmap32BufferedIterator() {
  this->destroy();
}

void RoaringBitmap32BufferedIterator::Init(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto stringN = v8::String::NewFromUtf8(isolate, "n", v8::NewStringType::kInternalized);
  auto className = v8::String::NewFromUtf8(isolate, "RoaringBitmap32BufferedIterator", v8::NewStringType::kInternalized);

  if (stringN.IsEmpty() || className.IsEmpty()) {
    return v8utils::throwError(isolate, "Failed to instantiate RoaringBitmap32BufferedIterator");
  }

  nPropertyName.Set(isolate, stringN.ToLocalChecked());

  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, New);

  ctor->SetClassName(className.ToLocalChecked());
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  constructorTemplate.Set(isolate, ctor);

  NODE_SET_PROTOTYPE_METHOD(ctor, "fill", fill);

  auto ctorFunction = ctor->GetFunction(isolate->GetCurrentContext());

  if (ctorFunction.IsEmpty()) {
    return v8utils::throwError(isolate, "Failed to instantiate RoaringBitmap32BufferedIterator");
  }

  auto ctorFunctionChecked = ctorFunction.ToLocalChecked();
  constructor.Set(isolate, ctorFunctionChecked);
  v8utils::defineHiddenField(isolate, exports, "RoaringBitmap32BufferedIterator", ctorFunctionChecked);
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

  RoaringBitmap32 * bitmapInstance = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], RoaringBitmap32::constructorTemplate, isolate);
  if (!bitmapInstance) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - first argument must be of type RoaringBitmap32");
  }

  auto bufferObject = info[1];

  if (!bufferObject->IsUint32Array()) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - second argument must be of type Uint32Array");
  }

  const v8utils::TypedArrayContent<uint32_t> bufferContent(bufferObject);
  if (!bufferContent.data || bufferContent.length < 1) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - invalid Uint32Array buffer");
  }

  RoaringBitmap32BufferedIterator * instance = new RoaringBitmap32BufferedIterator();
  if (!instance) {
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

    auto a0 = info[0]->ToObject(context);
    if (a0.IsEmpty()) {
      return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
    }
    instance->bitmap.Reset(isolate, a0.ToLocalChecked());

    auto a1 = bufferObject->ToObject(context);
    if (a1.IsEmpty()) {
      return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
    }
    instance->buffer.Reset(isolate, a1.ToLocalChecked());

    instance->bufferContent.set(bufferObject);
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

  RoaringBitmap32BufferedIterator * instance = v8utils::ObjectWrap::Unwrap<RoaringBitmap32BufferedIterator>(info.Holder());

  RoaringBitmap32 * bitmapInstance = instance->bitmapInstance;

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
    instance->buffer.Reset();
  }

  info.GetReturnValue().Set(n);
}

void RoaringBitmap32BufferedIterator::WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32BufferedIterator> const & info) {
  RoaringBitmap32BufferedIterator * p = info.GetParameter();
  delete p;
}
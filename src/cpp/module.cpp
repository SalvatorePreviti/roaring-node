#include "roaring.h"
#include "v8utils.h"

#include "RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"

void initTypes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  TypedArrays::initTypedArrays(isolate, info[0]->ToObject());
}

void InitModule(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  TypedArrays::initTypedArrays(isolate, Nan::GetCurrentContext()->Global());

  v8utils::defineHiddenFunction(exports, "_initTypes", initTypes);

  v8utils::defineHiddenField(exports, "default", exports);

  v8utils::defineReadonlyField(exports, "CRoaringVersion", v8::String::NewFromUtf8(isolate, ROARING_VERSION_STRING));

#ifdef USESSE4
  v8utils::defineReadonlyField(exports, "SSE42", v8::Boolean::New(isolate, true));
#ifdef USEAVX
  v8utils::defineReadonlyField(exports, "AVX2", v8::Boolean::New(isolate, true));
  v8utils::defineReadonlyField(exports, "instructionSet", v8::String::NewFromUtf8(isolate, "AVX2"));
#else
  v8utils::defineReadonlyField(exports, "AVX2", v8::Boolean::New(isolate, false));
  v8utils::defineReadonlyField(exports, "instructionSet", v8::String::NewFromUtf8(isolate, "SSE42"));
#endif

#else

  v8utils::defineReadonlyField(exports, "SSE42", v8::Boolean::New(isolate, false));
  v8utils::defineReadonlyField(exports, "AVX2", v8::Boolean::New(isolate, false));
  v8utils::defineReadonlyField(exports, "instructionSet", v8::String::NewFromUtf8(isolate, "PLAIN"));
#endif

  RoaringBitmap32::Init(exports);
  RoaringBitmap32Iterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

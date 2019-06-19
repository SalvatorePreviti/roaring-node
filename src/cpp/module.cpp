#include "roaring.h"

#include "RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32BufferedIterator/RoaringBitmap32BufferedIterator.h"

void initTypes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  JSTypes::initJSTypes(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
}

void InitModule(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  JSTypes::initJSTypes(isolate, isolate->GetCurrentContext()->Global());

  v8utils::defineHiddenFunction(isolate, exports, "_initTypes", initTypes);
  v8utils::defineHiddenField(isolate, exports, "default", exports);

#ifdef USESSE4
#  ifdef USEAVX
  v8utils::defineReadonlyField(isolate, exports, "SSE42", v8::Boolean::New(isolate, true));
  v8utils::defineReadonlyField(isolate, exports, "AVX2", v8::Boolean::New(isolate, true));
  v8utils::defineReadonlyField(isolate, exports, "instructionSet", v8::String::NewFromUtf8(isolate, "AVX2", v8::NewStringType::kInternalized).ToLocalChecked());
#  else
  v8utils::defineReadonlyField(isolate, exports, "SSE42", v8::Boolean::New(isolate, true));
  v8utils::defineReadonlyField(isolate, exports, "AVX2", v8::Boolean::New(isolate, false));
  v8utils::defineReadonlyField(
      isolate, exports, "instructionSet", v8::String::NewFromUtf8(isolate, "SSE42", v8::NewStringType::kInternalized).ToLocalChecked());
#  endif
#else
  v8utils::defineReadonlyField(isolate, exports, "SSE42", v8::Boolean::New(isolate, false));
  v8utils::defineReadonlyField(isolate, exports, "AVX2", v8::Boolean::New(isolate, false));
  v8utils::defineReadonlyField(
      isolate, exports, "instructionSet", v8::String::NewFromUtf8(isolate, "PLAIN", v8::NewStringType::kInternalized).ToLocalChecked());
#endif

  v8utils::defineReadonlyField(
      isolate, exports, "CRoaringVersion", v8::String::NewFromUtf8(isolate, ROARING_VERSION_STRING, v8::NewStringType::kInternalized).ToLocalChecked());

  RoaringBitmap32::Init(exports);
  RoaringBitmap32BufferedIterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

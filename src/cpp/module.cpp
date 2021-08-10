#include "roaring.h"

#include <stdio.h>
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

  char version[32];
  snprintf(version, 32, "%d.%d.%d", ROARING_VERSION_MAJOR, ROARING_VERSION_MINOR, ROARING_VERSION_REVISION);

  v8utils::defineReadonlyField(
      isolate, exports, "CRoaringVersion", v8::String::NewFromUtf8(isolate, version, v8::NewStringType::kInternalized).ToLocalChecked());

  RoaringBitmap32::Init(exports);
  RoaringBitmap32BufferedIterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

#include "roaring.h"
#include "v8utils.h"

#include "RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"

void initTypes(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  TypedArrays::initTypedArrays(info[0]->ToObject());
}

void InitModule(v8::Local<v8::Object> exports) {
  TypedArrays::initTypedArrays(Nan::GetCurrentContext()->Global());

  v8utils::defineHiddenFunction(exports, "_initTypes", initTypes);
  v8utils::defineHiddenField(exports, "default", exports);
  v8utils::defineReadonlyField(exports, "CRoaringVersion",
      Nan::New(std::to_string(ROARING_VERSION_MAJOR) + "." + std::to_string(ROARING_VERSION_MINOR) + "." + std::to_string(ROARING_VERSION_REVISION))
          .ToLocalChecked());

#ifdef USESSE4
  v8utils::defineReadonlyField(exports, "SSE42", Nan::New(true));
#ifdef USEAVX
  v8utils::defineReadonlyField(exports, "AVX2", Nan::New(true));
  v8utils::defineReadonlyField(exports, "instructionSet", Nan::New("AVX2").ToLocalChecked());
#else
  v8utils::defineReadonlyField(exports, "AVX2", Nan::New(false));
  v8utils::defineReadonlyField(exports, "instructionSet", Nan::New("SSE42").ToLocalChecked());
#endif

#else
  v8utils::defineReadonlyField(exports, "SSE42", Nan::New(false));
  v8utils::defineReadonlyField(exports, "AVX2", Nan::New(false));
  v8utils::defineReadonlyField(exports, "instructionSet", Nan::New("PLAIN").ToLocalChecked());
#endif

  RoaringBitmap32::Init(exports);
  RoaringBitmap32Iterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

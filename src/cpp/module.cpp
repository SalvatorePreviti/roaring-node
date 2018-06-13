#include "roaring.h"
#include "v8utils.h"

#include "RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"
#include "TypedArrays.h"

void initTypes(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  TypedArrays::initTypedArrays(info[0]->ToObject());
}

enum {
#ifdef USEAVX
  useavx = 1,
#else
  useavx = 0,
#endif

#ifdef USESSE4
  usesse4 = 1,
#else
  usesse4 = 0,
#endif
};

void InitModule(v8::Local<v8::Object> exports) {
  TypedArrays::initTypedArrays(Nan::GetCurrentContext()->Global());

  v8utils::defineHiddenFunction(exports, "_initTypes", initTypes);
  v8utils::defineHiddenField(exports, "default", exports);
  v8utils::defineReadonlyField(exports, "CRoaringVersion",
      Nan::New(std::to_string(ROARING_VERSION_MAJOR) + "." + std::to_string(ROARING_VERSION_MINOR) + "." + std::to_string(ROARING_VERSION_REVISION))
          .ToLocalChecked());

  v8utils::defineReadonlyField(exports, "AVX2", Nan::New(!!useavx));
  v8utils::defineReadonlyField(exports, "SSE42", Nan::New(!!usesse4));

  RoaringBitmap32::Init(exports);
  RoaringBitmap32Iterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

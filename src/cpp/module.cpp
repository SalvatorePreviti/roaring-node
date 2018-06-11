#include "roaring.h"
#include "v8utils.h"

#include "RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"
#include "TypedArrays.h"

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

  RoaringBitmap32::Init(exports);
  RoaringBitmap32Iterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

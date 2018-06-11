#include "roaring.h"
#include "v8utils.h"

#include "RoaringBitmap32/RoaringBitmap32.h"
#include "RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"
#include "TypedArrays.h"

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define ROARING_VERSION_STRING STRINGIZE(ROARING_VERSION_MAJOR) "." STRINGIZE(ROARING_VERSION_MINOR) "." STRINGIZE(ROARING_VERSION_REVISION)

void initTypes(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  TypedArrays::initTypedArrays(info[0]->ToObject());
}

void InitModule(v8::Local<v8::Object> exports) {
  TypedArrays::initTypedArrays(Nan::GetCurrentContext()->Global());

  v8utils::defineHiddenFunction(exports, "_initTypes", initTypes);
  v8utils::defineHiddenField(exports, "default", exports);
  v8utils::defineReadonlyField(exports, "version", Nan::New(ROARING_VERSION_STRING).ToLocalChecked());

  RoaringBitmap32::Init(exports);
  RoaringBitmap32Iterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

#include "v8utils.h"

#include "RoaringBitmap32.h"
#include "RoaringBitmap32Iterator.h"

void InitModule(v8::Local<v8::Object> exports) {
  v8utils::defineHiddenField(exports, "default", exports);

  RoaringBitmap32::Init(exports);
  RoaringBitmap32Iterator::Init(exports);
}

NODE_MODULE(roaring, InitModule);

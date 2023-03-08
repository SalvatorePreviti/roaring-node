#include "aligned-buffers.h"
#include "RoaringBitmap32-main.h"
#include "RoaringBitmap32BufferedIterator.h"

void InitModule(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  v8::HandleScope scope(isolate);

  globalAddonData.initialize(isolate);
  v8utils::defineHiddenField(isolate, exports, "default", exports);

  NODE_SET_METHOD(exports, "getRoaringUsedMemory", getRoaringUsedMemory);

  AlignedBuffers_Init(exports);
  RoaringBitmap32_Init(exports, &globalAddonData);
  RoaringBitmap32BufferedIterator_Init(exports, &globalAddonData);
}

NODE_MODULE(roaring, InitModule);

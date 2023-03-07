#include "RoaringBitmap32.cpp"
#include "RoaringBitmap32BufferedIterator.h"

static void InitModule(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  globalAddonData.initialize(isolate);
  v8utils::defineHiddenField(isolate, exports, "default", exports);

  NODE_SET_METHOD(exports, "bufferAlignedAlloc", bufferAlignedAlloc);
  NODE_SET_METHOD(exports, "bufferAlignedAllocUnsafe", bufferAlignedAllocUnsafe);
  NODE_SET_METHOD(exports, "isBufferAligned", isBufferAligned);
  NODE_SET_METHOD(exports, "getRoaringUsedMemory", getRoaringUsedMemory);

  RoaringBitmap32_Init(exports);
  RoaringBitmap32BufferedIterator_Init(exports);
}

NODE_MODULE(roaring, InitModule);

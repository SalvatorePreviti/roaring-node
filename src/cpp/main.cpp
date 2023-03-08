#include "aligned-buffers.h"
#include "RoaringBitmap32-main.h"
#include "RoaringBitmap32BufferedIterator.h"

using namespace v8;

void AddonData_DeleteInstance(void * addonData) {
  if (thread_local_isolate == reinterpret_cast<AddonData *>(addonData)->isolate) {
    thread_local_isolate = nullptr;
  }

  delete (AddonData *)addonData;
}

extern "C" NODE_MODULE_EXPORT void NODE_MODULE_INITIALIZER(Local<Object> exports
                                                           // Local<Value> module
                                                           // Local<Context> context
) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  thread_local_isolate = isolate;

  v8::HandleScope scope(isolate);

  AddonData * addonData = new AddonData();

  node::AddEnvironmentCleanupHook(isolate, AddonData_DeleteInstance, addonData);

  addonData->initialize(isolate);

  AlignedBuffers_Init(exports, addonData);
  RoaringBitmap32_Init(exports, addonData);
  RoaringBitmap32BufferedIterator_Init(exports, addonData);

  AddonData_setMethod(exports, "getRoaringUsedMemory", getRoaringUsedMemory, addonData);

  v8utils::defineHiddenField(isolate, exports, "default", exports);
}

// NODE_MODULE(roaring, InitModule);

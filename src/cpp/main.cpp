#include "aligned-buffers.h"
#include "RoaringBitmap32-main.h"
#include "RoaringBitmap32BufferedIterator.h"

using namespace v8;

#define PREPROCESSOR_CONCAT(a, b) PREPROCESSOR_CONCAT_HELPER(a, b)
#define PREPROCESSOR_CONCAT_HELPER(a, b) a##b

#if NODE_MAJOR_VERSION >= 11
#  define MODULE_WORKER_ENABLED(module_name, registration)                                               \
    extern "C" NODE_MODULE_EXPORT void PREPROCESSOR_CONCAT(node_register_module_v, NODE_MODULE_VERSION)( \
      v8::Local<v8::Object> exports, v8::Local<v8::Value> module, v8::Local<v8::Context> context) {      \
      registration(exports);                                                                             \
    }
#else
#  define MODULE_WORKER_ENABLED(module_name, registration) NODE_MODULE(module_name, registration)
#endif

void AddonData_DeleteInstance(void * addonData) {
  if (thread_local_isolate == reinterpret_cast<AddonData *>(addonData)->isolate) {
    thread_local_isolate = nullptr;
  }

  delete (AddonData *)addonData;
}

void InitRoaringNode(Local<Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  thread_local_isolate = isolate;

  v8::HandleScope scope(isolate);

  AddonData * addonData = new AddonData();

#if NODE_MAJOR_VERSION >= 11
  node::AddEnvironmentCleanupHook(isolate, AddonData_DeleteInstance, addonData);
#endif

  addonData->initialize(isolate);

  AlignedBuffers_Init(exports, addonData);
  RoaringBitmap32_Init(exports, addonData);
  RoaringBitmap32BufferedIterator_Init(exports, addonData);

  AddonData_setMethod(exports, "getRoaringUsedMemory", getRoaringUsedMemory, addonData);

  v8utils::defineHiddenField(isolate, exports, "default", exports);
}

MODULE_WORKER_ENABLED(roaring, InitRoaringNode);

#include "aligned-buffers.h"
#include "RoaringBitmap32-main.h"
#include "RoaringBitmap32BufferedIterator.h"
#include <mutex>

using namespace v8;

#define PREPROCESSOR_CONCAT(a, b) PREPROCESSOR_CONCAT_HELPER(a, b)
#define PREPROCESSOR_CONCAT_HELPER(a, b) a##b

#define MODULE_WORKER_ENABLED(module_name, registration)                                               \
  extern "C" NODE_MODULE_EXPORT void PREPROCESSOR_CONCAT(node_register_module_v, NODE_MODULE_VERSION)( \
    v8::Local<v8::Object> exports, v8::Local<v8::Value> module, v8::Local<v8::Context> context) {      \
    registration(exports);                                                                             \
  }

void InitRoaringNode(Local<Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  v8::HandleScope scope(isolate);

  AddonData * addonData = new AddonData(isolate);

  ignoreMaybeResult(exports->Set(
    isolate->GetCurrentContext(),
    v8::String::NewFromUtf8Literal(isolate, "default", v8::NewStringType::kInternalized),
    exports));

  AlignedBuffers_Init(exports, addonData);
  RoaringBitmap32_Init(exports, addonData);
  RoaringBitmap32BufferedIterator_Init(exports, addonData);

  addonData->setStaticMethod(exports, "getRoaringUsedMemory", getRoaringUsedMemory);

  addonData->initializationCompleted();
}

MODULE_WORKER_ENABLED(roaring, InitRoaringNode);

#include "croaring.cpp"

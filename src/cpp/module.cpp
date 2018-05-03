#include "v8utils.h"

#include "RoaringBitmap32.h"

NAN_MODULE_INIT(InitModule) {
  v8utils::defineHiddenField(target, "default", target);

  RoaringBitmap32::Init(target);
}

NODE_MODULE(roaring, InitModule);

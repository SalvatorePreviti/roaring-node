#ifndef ROARING_NODE_ADDON_STRINGS_
#define ROARING_NODE_ADDON_STRINGS_

#include "includes.h"

const char * const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";
const char * const ERROR_INVALID_OBJECT = "Invalid RoaringBitmap32 object";

class AddonDataStrings final {
 public:
  v8::Persistent<v8::String, v8::CopyablePersistentTraits<v8::String>> n;
  v8::Persistent<v8::String, v8::CopyablePersistentTraits<v8::String>> readonly;

  v8::Persistent<v8::String, v8::CopyablePersistentTraits<v8::String>> CRoaringVersionValue;

  inline void initialize(v8::Isolate * isolate) {
    literal(isolate, this->n, "n");
    literal(isolate, this->readonly, "readonly");

    literal(isolate, CRoaringVersionValue, ROARING_VERSION);
  }

 private:
  template <int N>
  static void literal(
    v8::Isolate * isolate,
    v8::Persistent<v8::String, v8::CopyablePersistentTraits<v8::String>> & result,
    const char (&literal)[N]) {
    result.Reset(isolate, v8::String::NewFromUtf8Literal(isolate, literal, v8::NewStringType::kInternalized));
  }
};

#endif

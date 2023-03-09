#ifndef ROARING_NODE_ADDON_STRINGS_
#define ROARING_NODE_ADDON_STRINGS_

#include "includes.h"

const char * const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";
const char * const ERROR_INVALID_OBJECT = "Invalid RoaringBitmap32 object";

class AddonDataStrings final {
 public:
  v8::Eternal<v8::String> n;
  v8::Eternal<v8::String> readonly;
  v8::Eternal<v8::String> RoaringBitmap32;
  v8::Eternal<v8::Symbol> symbol_rnshared;

  v8::Eternal<v8::String> OperationFailed;

  inline void initialize(v8::Isolate * isolate) {
    literal(isolate, this->n, "n");
    literal(isolate, this->readonly, "readonly");
    literal(isolate, this->RoaringBitmap32, "RoaringBitmap32");

    literal(isolate, this->OperationFailed, "Operation failed");

    symbol_rnshared.Set(
      isolate, v8::Symbol::ForApi(isolate, NEW_LITERAL_V8_STRING(isolate, "rnshared", v8::NewStringType::kInternalized)));
  }

 private:
  template <int N>
  static void literal(v8::Isolate * isolate, v8::Eternal<v8::String> & result, const char (&literal)[N]) {
    result.Set(isolate, NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized));
  }
};

#endif

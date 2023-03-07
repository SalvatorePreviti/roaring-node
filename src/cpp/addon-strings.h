#ifndef ROARING_NODE_ADDON_STRINGS_
#define ROARING_NODE_ADDON_STRINGS_

#include "includes.h"

const char * const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";
const char * const ERROR_INVALID_OBJECT = "Invalid RoaringBitmap32 object";

class AddonDataStrings {
 public:
  v8::Eternal<v8::String> n;
  v8::Eternal<v8::String> readonly;
  v8::Eternal<v8::String> RoaringBitmap32;

  v8::Eternal<v8::String> OperationFailed;

  void initialize(v8::Isolate * isolate) {
    this->literal(isolate, this->n, "n");
    this->literal(isolate, this->readonly, "readonly");
    this->literal(isolate, this->RoaringBitmap32, "RoaringBitmap32");

    this->literal(isolate, this->OperationFailed, "Operation failed");
  }

 private:
  template <int N>
  void literal(v8::Isolate * isolate, v8::Eternal<v8::String> & result, const char (&literal)[N]) {
    result.Set(isolate, NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized));
  }
};

#endif

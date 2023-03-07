#ifndef ROARING_NODE_ADDON_STRINGS_
#define ROARING_NODE_ADDON_STRINGS_

#include "includes.h"

class AddonDataStrings {
 public:
  v8::Eternal<v8::String> n;
  v8::Eternal<v8::String> readonly;

  void initialize(v8::Isolate * isolate) {
    this->literal(isolate, this->n, "n");
    this->literal(isolate, this->readonly, "readonly");
  }

 private:
  template <int N>
  void literal(v8::Isolate * isolate, v8::Eternal<v8::String> & result, const char (&literal)[N]) {
    result.Set(isolate, NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized));
  }
};

#endif

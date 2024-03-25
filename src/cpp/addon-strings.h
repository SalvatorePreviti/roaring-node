#ifndef ROARING_NODE_ADDON_STRINGS_
#define ROARING_NODE_ADDON_STRINGS_

#include "includes.h"

const char * const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";
const char * const ERROR_INVALID_OBJECT = "Invalid RoaringBitmap32 object";

class AddonDataStrings final {
 public:
  v8::Persistent<v8::String> n;
  v8::Persistent<v8::String> _default;
  v8::Persistent<v8::String> readonly;
  v8::Persistent<v8::String> RoaringBitmap32;
  v8::Persistent<v8::String> RoaringBitmap32BufferedIterator;
  v8::Persistent<v8::Symbol> addonDataSym;

  v8::Persistent<v8::String> OperationFailed;
  v8::Persistent<v8::String> Comma;

  v8::Persistent<v8::String> Uint32Array;
  v8::Persistent<v8::String> Buffer;

  v8::Persistent<v8::String> from;

  v8::Persistent<v8::String> containers;
  v8::Persistent<v8::String> arrayContainers;
  v8::Persistent<v8::String> runContainers;
  v8::Persistent<v8::String> bitsetContainers;
  v8::Persistent<v8::String> valuesInArrayContainers;
  v8::Persistent<v8::String> valuesInRunContainers;
  v8::Persistent<v8::String> valuesInBitsetContainers;
  v8::Persistent<v8::String> bytesInArrayContainers;
  v8::Persistent<v8::String> bytesInRunContainers;
  v8::Persistent<v8::String> bytesInBitsetContainers;
  v8::Persistent<v8::String> maxValue;
  v8::Persistent<v8::String> minValue;
  v8::Persistent<v8::String> sumOfAllValues;
  v8::Persistent<v8::String> size;
  v8::Persistent<v8::String> isFrozen;
  v8::Persistent<v8::String> isEmpty;

  v8::Persistent<v8::String> CRoaringVersion;
  v8::Persistent<v8::String> CRoaringVersionValue;

  inline void initialize(v8::Isolate * isolate) {
    literal(isolate, this->n, "n");
    literal(isolate, this->_default, "default");
    literal(isolate, this->readonly, "readonly");
    literal(isolate, this->RoaringBitmap32, "RoaringBitmap32");
    literal(isolate, this->RoaringBitmap32BufferedIterator, "RoaringBitmap32BufferedIterator");

    literal(isolate, this->OperationFailed, "Operation failed");
    literal(isolate, this->Comma, ",");

    literal(isolate, this->Uint32Array, "Uint32Array");
    literal(isolate, this->Buffer, "Buffer");

    literal(isolate, from, "from");

    literal(isolate, containers, "containers");
    literal(isolate, arrayContainers, "arrayContainers");
    literal(isolate, runContainers, "runContainers");
    literal(isolate, bitsetContainers, "bitsetContainers");
    literal(isolate, valuesInArrayContainers, "valuesInArrayContainers");
    literal(isolate, valuesInRunContainers, "valuesInRunContainers");
    literal(isolate, valuesInBitsetContainers, "valuesInBitsetContainers");
    literal(isolate, bytesInArrayContainers, "bytesInArrayContainers");
    literal(isolate, bytesInRunContainers, "bytesInRunContainers");
    literal(isolate, bytesInBitsetContainers, "bytesInBitsetContainers");
    literal(isolate, maxValue, "maxValue");
    literal(isolate, minValue, "minValue");
    literal(isolate, sumOfAllValues, "sumOfAllValues");
    literal(isolate, size, "size");
    literal(isolate, isFrozen, "isFrozen");
    literal(isolate, isEmpty, "isEmpty");
    literal(isolate, CRoaringVersion, "CRoaringVersion");
    literal(isolate, CRoaringVersionValue, ROARING_VERSION);
  }

 private:
  template <int N>
  static void literal(v8::Isolate * isolate, v8::Persistent<v8::String> & result, const char (&literal)[N]) {
    result.Reset(isolate, v8::String::NewFromUtf8Literal(isolate, literal, v8::NewStringType::kInternalized));
  }
};

#endif

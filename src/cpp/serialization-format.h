#ifndef ROARING_NODE_SERIALIZATION_FORMAT_
#define ROARING_NODE_SERIALIZATION_FORMAT_

#include "includes.h"

const uint32_t MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES = 0x00FFFFFF;

enum class SerializationFormat {
  INVALID = -1,
  croaring = 0,
  portable = 1,
  unsafe_frozen_croaring = 2,
};

enum class DeserializationFormat {
  INVALID = -1,
  croaring = 0,
  portable = 1,
  unsafe_frozen_croaring = 2,
  unsafe_frozen_portable = 3,
};

enum class FrozenViewFormat {
  INVALID = -1,
  unsafe_frozen_croaring = 2,
  unsafe_frozen_portable = 3,
};

static SerializationFormat tryParseSerializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  if (!isolate || value.IsEmpty()) {
    return SerializationFormat::INVALID;
  }
  if (value->IsBoolean()) {
    if (value->IsTrue()) {
      return SerializationFormat::portable;
    }
    if (value->IsFalse()) {
      return SerializationFormat::croaring;
    }
  } else if (value->IsString()) {
#if NODE_MAJOR_VERSION > 8
    v8::String::Utf8Value formatString(isolate, value);
#else
    v8::String::Utf8Value formatString(value);
#endif
    if (strcmp(*formatString, "croaring") == 0) {
      return SerializationFormat::croaring;
    }
    if (strcmp(*formatString, "portable") == 0) {
      return SerializationFormat::portable;
    }
    if (strcmp(*formatString, "unsafe_frozen_croaring") == 0) {
      return SerializationFormat::unsafe_frozen_croaring;
    }
  }
  return SerializationFormat::INVALID;
}

static DeserializationFormat tryParseDeserializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  if (!isolate || value.IsEmpty()) {
    return DeserializationFormat::INVALID;
  }
  if (value->IsBoolean()) {
    if (value->IsTrue()) {
      return DeserializationFormat::portable;
    }
    if (value->IsFalse()) {
      return DeserializationFormat::croaring;
    }
  } else if (value->IsString()) {
#if NODE_MAJOR_VERSION > 8
    v8::String::Utf8Value formatString(isolate, value);
#else
    v8::String::Utf8Value formatString(value);
#endif
    if (strcmp(*formatString, "croaring") == 0) {
      return DeserializationFormat::croaring;
    }
    if (strcmp(*formatString, "portable") == 0) {
      return DeserializationFormat::portable;
    }
    if (strcmp(*formatString, "unsafe_frozen_croaring") == 0) {
      return DeserializationFormat::unsafe_frozen_croaring;
    }
    if (strcmp(*formatString, "unsafe_frozen_portable") == 0) {
      return DeserializationFormat::unsafe_frozen_portable;
    }
  }
  return DeserializationFormat::INVALID;
}

static FrozenViewFormat tryParseFrozenViewFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  if (!isolate || value.IsEmpty()) {
    return FrozenViewFormat::INVALID;
  }
  if (value->IsString()) {
#if NODE_MAJOR_VERSION > 8
    v8::String::Utf8Value formatString(isolate, value);
#else
    v8::String::Utf8Value formatString(value);
#endif
    if (strcmp(*formatString, "unsafe_frozen_croaring") == 0) {
      return FrozenViewFormat::unsafe_frozen_croaring;
    }
    if (strcmp(*formatString, "unsafe_frozen_portable") == 0) {
      return FrozenViewFormat::unsafe_frozen_portable;
    }
  }
  return FrozenViewFormat::INVALID;
}

#endif  // ROARING_NODE_SERIALIZATION_FORMAT_

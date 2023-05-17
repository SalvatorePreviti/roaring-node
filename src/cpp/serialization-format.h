#ifndef ROARING_NODE_SERIALIZATION_FORMAT_
#define ROARING_NODE_SERIALIZATION_FORMAT_

#include "includes.h"

const uint32_t MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES = 0x00FFFFFF;

enum class SerializationFormat {
  INVALID = -1,
  croaring = 0,
  portable = 1,
  unsafe_frozen_croaring = 2,
  uint32_array = 4,
};

enum class FileSerializationFormat {
  INVALID = -1,
  croaring = 0,
  portable = 1,
  unsafe_frozen_croaring = 2,
  uint32_array = 4,

  comma_separated_values = 10,
  tab_separated_values = 11,
  newline_separated_values = 12,
  json_array = 20
};

enum class DeserializationFormat {
  INVALID = -1,
  croaring = 0,
  portable = 1,
  unsafe_frozen_croaring = 2,
  unsafe_frozen_portable = 3,
  uint32_array = 4,
};

enum class FrozenViewFormat {
  INVALID = -1,
  unsafe_frozen_croaring = 2,
  unsafe_frozen_portable = 3,
};

SerializationFormat tryParseSerializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
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
    v8::String::Utf8Value formatString(isolate, value);
    if (strcmp(*formatString, "croaring") == 0) {
      return SerializationFormat::croaring;
    }
    if (strcmp(*formatString, "portable") == 0) {
      return SerializationFormat::portable;
    }
    if (strcmp(*formatString, "unsafe_frozen_croaring") == 0) {
      return SerializationFormat::unsafe_frozen_croaring;
    }
    if (strcmp(*formatString, "uint32_array") == 0) {
      return SerializationFormat::uint32_array;
    }
  }
  return SerializationFormat::INVALID;
}

FileSerializationFormat tryParseFileSerializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  SerializationFormat sf = tryParseSerializationFormat(value, isolate);
  if (sf != SerializationFormat::INVALID) {
    return static_cast<FileSerializationFormat>(sf);
  }
  if (!isolate || value.IsEmpty()) {
    return FileSerializationFormat::INVALID;
  }
  if (value->IsString()) {
    v8::String::Utf8Value formatString(isolate, value);
    if (strcmp(*formatString, "comma_separated_values") == 0) {
      return FileSerializationFormat::comma_separated_values;
    }
    if (strcmp(*formatString, "tab_separated_values") == 0) {
      return FileSerializationFormat::tab_separated_values;
    }
    if (strcmp(*formatString, "newline_separated_values") == 0) {
      return FileSerializationFormat::newline_separated_values;
    }
    if (strcmp(*formatString, "json_array") == 0) {
      return FileSerializationFormat::json_array;
    }
  }
  return FileSerializationFormat::INVALID;
}

DeserializationFormat tryParseDeserializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
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
    v8::String::Utf8Value formatString(isolate, value);
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
    if (strcmp(*formatString, "uint32_array") == 0) {
      return DeserializationFormat::uint32_array;
    }
  }
  return DeserializationFormat::INVALID;
}

FrozenViewFormat tryParseFrozenViewFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
  if (!isolate || value.IsEmpty()) {
    return FrozenViewFormat::INVALID;
  }
  if (value->IsString()) {
    v8::String::Utf8Value formatString(isolate, value);
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

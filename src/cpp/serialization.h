#ifndef ROARING_NODE_SERIALIZATION_
#define ROARING_NODE_SERIALIZATION_

#include "RoaringBitmap32.h"
#include "serialization-csv.h"
#include "mmap.h"

#if defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
#  include <io.h>
#endif

#ifndef CROARING_SERIALIZATION_ARRAY_UINT32
constexpr const unsigned char CROARING_SERIALIZATION_ARRAY_UINT32 = 1;
#endif

#ifndef CROARING_SERIALIZATION_CONTAINER
constexpr const unsigned char CROARING_SERIALIZATION_CONTAINER = 2;
#endif

#include <iostream>

class RoaringBitmapSerializerBase {
 private:
  bool serializeArray = false;
  size_t cardinality = 0;

 public:
  RoaringBitmap32 * self = nullptr;
  FileSerializationFormat format = FileSerializationFormat::INVALID;

  size_t volatile serializedSize = 0;

  WorkerError computeSerializedSize() {
    size_t buffersize;
    switch (this->format) {
      case FileSerializationFormat::croaring: {
        this->cardinality = this->self->getSize();
        auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);
        auto portablesize = roaring_bitmap_portable_size_in_bytes(this->self->roaring);
        if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
          buffersize = portablesize + 1;
        } else {
          this->serializeArray = true;
          buffersize = (size_t)sizeasarray + 1;
        }
        break;
      }

      case FileSerializationFormat::portable: {
        buffersize = roaring_bitmap_portable_size_in_bytes(this->self->roaring);
        break;
      }

      case FileSerializationFormat::unsafe_frozen_croaring: {
        buffersize = roaring_bitmap_frozen_size_in_bytes(this->self->roaring);
        break;
      }

      case FileSerializationFormat::uint32_array: {
        buffersize = this->self->getSize() * sizeof(uint32_t);
        break;
      }

      default: return WorkerError("RoaringBitmap32 serialization format is invalid");
    }

    this->serializedSize = buffersize;
    return WorkerError();
  }

  WorkerError serializeToBuffer(uint8_t * data) {
    if (!data) {
      return WorkerError("RoaringBitmap32 serialization allocation failed");
    }

    std::cout << "serializeToBuffer" << std::endl;

    switch (format) {
      case FileSerializationFormat::croaring: {
        if (serializeArray) {
          ((uint8_t *)data)[0] = CROARING_SERIALIZATION_ARRAY_UINT32;
          memcpy(data + 1, &this->cardinality, sizeof(uint32_t));
          roaring_bitmap_to_uint32_array(self->roaring, (uint32_t *)(data + 1 + sizeof(uint32_t)));
        } else {
          ((uint8_t *)data)[0] = CROARING_SERIALIZATION_CONTAINER;
          roaring_bitmap_portable_serialize(self->roaring, (char *)data + 1);
        }
        break;
      }

      case FileSerializationFormat::portable: {
        roaring_bitmap_portable_serialize(self->roaring, (char *)data);
        break;
      }

      case FileSerializationFormat::unsafe_frozen_croaring: {
        roaring_bitmap_frozen_serialize(self->roaring, (char *)data);
        break;
      }

      case FileSerializationFormat::uint32_array: {
        std::cout << "uint32_array" << std::endl;
        roaring_bitmap_to_uint32_array(self->roaring, (uint32_t *)data);
        std::cout << "uint32_array1" << std::endl;
        break;
      }

      default: return WorkerError("RoaringBitmap32 serialization format is invalid");
    }
    return WorkerError();
  }
};

class RoaringBitmapSerializer final : public RoaringBitmapSerializerBase {
 public:
  v8utils::TypedArrayContent<uint8_t> inputBuffer;
  uint8_t * volatile allocatedBuffer = nullptr;

  void parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Isolate * isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);
    int formatArgIndex = 0;
    int bufferArgIndex = -1;

    RoaringBitmap32 * bitmap = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
    if (bitmap == nullptr) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization on invalid object");
    }
    if (info.Length() <= 0) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization format argument was not provided");
    }
    if (info.Length() > 1) {
      if (
        info[0]->IsUint8Array() || info[0]->IsInt8Array() || info[0]->IsUint8ClampedArray() || info[0]->IsArrayBuffer() ||
        info[0]->IsSharedArrayBuffer()) {
        bufferArgIndex = 0;
        formatArgIndex = 1;
      } else if (
        info[1]->IsUint8Array() || info[1]->IsInt8Array() || info[1]->IsUint8ClampedArray() || info[1]->IsArrayBuffer() ||
        info[1]->IsSharedArrayBuffer()) {
        bufferArgIndex = 1;
      } else if (!info[1]->IsUndefined()) {
        return v8utils::throwError(isolate, "RoaringBitmap32 serialization buffer argument was invalid");
      }
    }
    this->format = static_cast<FileSerializationFormat>(tryParseSerializationFormat(info[formatArgIndex], isolate));
    if (this->format == FileSerializationFormat::INVALID) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization format argument was invalid");
    }
    if (bufferArgIndex >= 0) {
      if (!this->inputBuffer.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
        return v8utils::throwError(isolate, "RoaringBitmap32 serialization buffer argument was invalid");
      }
    }
    this->self = bitmap;
  }

  WorkerError serialize() {
    WorkerError err = this->computeSerializedSize();
    if (err.hasError()) {
      return err;
    }

    uint8_t * data = this->inputBuffer.data;

    if (data == nullptr) {
      data = (uint8_t *)bare_aligned_malloc(
        this->format == FileSerializationFormat::unsafe_frozen_croaring ? 32 : 8, this->serializedSize);
      this->allocatedBuffer = data;
    } else if (this->inputBuffer.length < this->serializedSize) {
      return WorkerError("RoaringBitmap32 serialization buffer is too small");
    }

    return this->serializeToBuffer(data);
  }

  void done(v8::Isolate * isolate, v8::Local<v8::Value> & result) {
    if (!this->self) {
      return;
    }
    uint8_t * allocatedBuffer = this->allocatedBuffer;

    if (allocatedBuffer) {
      // Create a new buffer using the allocated memory
      v8::MaybeLocal<v8::Object> nodeBufferMaybeLocal =
        node::Buffer::New(isolate, (char *)allocatedBuffer, this->serializedSize, bare_aligned_free_callback, nullptr);
      if (!nodeBufferMaybeLocal.ToLocal(&result)) {
        return v8utils::throwError(isolate, "RoaringBitmap32 serialization failed to create a new buffer");
      }
      this->allocatedBuffer = nullptr;
      return;
    }

    if (!v8utils::v8ValueToBufferWithLimit(
          isolate, self->addonData, this->inputBuffer.bufferPersistent.Get(isolate), this->serializedSize, result)) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization failed to create the buffer view");
    }
  }

  ~RoaringBitmapSerializer() { bare_aligned_free(this->allocatedBuffer); }
};

class RoaringBitmapFileSerializer final : public RoaringBitmapSerializerBase {
 public:
  std::string filePath;

  void parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Isolate * isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    RoaringBitmap32 * bitmap = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
    if (bitmap == nullptr) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization on invalid object");
    }
    if (info.Length() < 2) {
      return v8utils::throwError(isolate, "RoaringBitmap32::serializeFileAsync requires 2 arguments");
    }
    if (!info[0]->IsString()) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization file path argument was invalid");
    }

    this->format = tryParseFileSerializationFormat(info[1], isolate);
    if (this->format == FileSerializationFormat::INVALID) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization format argument was invalid");
    }

    v8::String::Utf8Value filePathUtf8(isolate, info[0]);
    this->filePath = std::string(*filePathUtf8, filePathUtf8.length());
    this->self = bitmap;
  }

  WorkerError serialize() {
    std::cout << "serialize" << std::endl;

    switch (this->format) {
      case FileSerializationFormat::comma_separated_values:
      case FileSerializationFormat::tab_separated_values:
      case FileSerializationFormat::newline_separated_values:
      case FileSerializationFormat::json_array: {
        int fd = open(this->filePath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) {
          return WorkerError::from_errno("open", this->filePath);
        }
        int errorno = CsvFileDescriptorSerializer::iterate(this->self->roaring, fd, this->format);
        close(fd);
        return errorno != 0 ? WorkerError(errorno, "write", this->filePath) : WorkerError();
      }

      default: break;
    }

    std::cout << "serialize1" << std::endl;

    WorkerError err = this->computeSerializedSize();
    if (err.hasError()) {
      return err;
    }

    int fd = open(this->filePath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
      return WorkerError::from_errno("open", this->filePath);
    }

#if defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
    int truncateErr = _chsize_s(fd, this->serializedSize);
    if (truncateErr != 0) {
      err = WorkerError(truncateErr, "_chsize_s", this->filePath);
      close(fd);
      return err;
    }
#else
    if (ftruncate(fd, this->serializedSize) < 0) {
      err = WorkerError::from_errno("ftruncate", this->filePath);
      close(fd);
      return err;
    }
#endif

    if (this->serializedSize != 0) {
      uint8_t * data = (uint8_t *)mmap(nullptr, this->serializedSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      std::cout << "after mmap " << this->serializedSize << std::endl;
      std::cout << "data" << (void *)data << std::endl;
      if (data == MAP_FAILED) {
        std::cout << "mmap failed" << this->serializedSize << std::endl;

        // mmap failed, allocate and write to buffer instead
        data = (uint8_t *)gcaware_aligned_malloc(this->serializedSize, 32);
        if (data) {
          err = this->serializeToBuffer(data);
          if (!err.hasError()) {
            auto wresult = write(fd, data, this->serializedSize);
            std::cout << "wresult" << wresult << std::endl;
            if (wresult < 0) {
              err = WorkerError::from_errno("write", this->filePath);
              close(fd);
            }
          }
          gcaware_aligned_free(data);
          return err;
        }

        err = WorkerError::from_errno("mmap", this->filePath);
        close(fd);
        return err;
      }

      err = this->serializeToBuffer(data);

      if (err.hasError()) {
        close(fd);
        return err;
      }

      munmap(data, this->serializedSize);
    }

    close(fd);
    return err;
  }
};

class RoaringBitmapDeserializerBase {
 public:
  DeserializationFormat format = DeserializationFormat::INVALID;
  v8::Isolate * isolate = nullptr;
  roaring_bitmap_t_ptr volatile roaring = nullptr;
  uint8_t * volatile frozenBuffer = nullptr;

  ~RoaringBitmapDeserializerBase() {
    if (this->frozenBuffer != nullptr) {
      bare_aligned_free(this->frozenBuffer);
    }
    if (this->roaring) {
      roaring_bitmap_free(this->roaring);
    }
  }

  WorkerError deserializeBuf(const char * bufaschar, size_t bufLen) {
    if (this->format == DeserializationFormat::INVALID) {
      return WorkerError("RoaringBitmap32 deserialization format argument was invalid");
    }

    if (bufLen == 0 || !bufaschar) {
      // Empty bitmap for an empty buffer.
      this->roaring = roaring_bitmap_create();
      if (!this->roaring) {
        return WorkerError("RoaringBitmap32 deserialization failed to create an empty bitmap");
      }
      return WorkerError();
    }

    switch (this->format) {
      case DeserializationFormat::portable: {
        this->roaring = roaring_bitmap_portable_deserialize_safe(bufaschar, bufLen);
        if (!this->roaring) {
          return WorkerError("RoaringBitmap32 deserialization - portable deserialization failed");
        }
        return WorkerError();
      }

      case DeserializationFormat::croaring: {
        switch ((unsigned char)bufaschar[0]) {
          case CROARING_SERIALIZATION_ARRAY_UINT32: {
            uint32_t card;
            memcpy(&card, bufaschar + 1, sizeof(uint32_t));

            if (card * sizeof(uint32_t) + sizeof(uint32_t) + 1 != bufLen) {
              return WorkerError("RoaringBitmap32 deserialization corrupted data, wrong cardinality header");
            }

            const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
            this->roaring = roaring_bitmap_of_ptr(card, elems);
            if (!this->roaring) {
              return WorkerError("RoaringBitmap32 deserialization - uint32 array deserialization failed");
            }
            return WorkerError();
          }

          case CROARING_SERIALIZATION_CONTAINER: {
            this->roaring = roaring_bitmap_portable_deserialize_safe(bufaschar + 1, bufLen - 1);
            if (!this->roaring) {
              return WorkerError("RoaringBitmap32 deserialization - container deserialization failed");
            }
            return WorkerError();
          }
        }

        return WorkerError("RoaringBitmap32 deserialization - invalid portable header byte");
      }

      case DeserializationFormat::unsafe_frozen_portable:
      case DeserializationFormat::unsafe_frozen_croaring: {
        this->frozenBuffer = (uint8_t *)bare_aligned_malloc(32, bufLen);
        if (!this->frozenBuffer) {
          return WorkerError("RoaringBitmap32 deserialization - failed to allocate memory for frozen bitmap");
        }
        memcpy(this->frozenBuffer, bufaschar, bufLen);

        if (format == DeserializationFormat::unsafe_frozen_croaring) {
          this->roaring =
            const_cast<roaring_bitmap_t_ptr>(roaring_bitmap_frozen_view((const char *)this->frozenBuffer, bufLen));
          return this->roaring ? WorkerError()
                               : WorkerError("RoaringBitmap32 deserialization - failed to create a frozen view");
        }

        this->roaring =
          const_cast<roaring_bitmap_t_ptr>(roaring_bitmap_portable_deserialize_frozen((const char *)this->frozenBuffer));
        if (!this->roaring) {
          return WorkerError("RoaringBitmap32 deserialization - failed to create a frozen view");
        }
        return WorkerError();
      }

      case DeserializationFormat::uint32_array: {
        if (bufLen % 4 != 0) {
          return WorkerError(
            "RoaringBitmap32 deserialization - uint32 array deserialization failed, input length is not a multiple of 4");
        }

        if (bufLen == 0) {
          this->roaring = roaring_bitmap_create();
          if (!this->roaring) {
            return WorkerError("RoaringBitmap32 deserialization failed to create an empty bitmap");
          }
          return WorkerError();
        }

        this->roaring = roaring_bitmap_of_ptr(bufLen >> 2, (const uint32_t *)bufaschar);
        if (!this->roaring) {
          return WorkerError("RoaringBitmap32 deserialization - uint32 array deserialization failed");
        }
        return WorkerError();
      }

      default: return WorkerError("RoaringBitmap32 deserialization - unknown deserialization format");
    }
  }

  void finalizeTargetBitmap(RoaringBitmap32 * targetBitmap) {
    targetBitmap->replaceBitmapInstance(this->isolate, this->roaring);
    this->roaring = nullptr;

    if (this->frozenBuffer) {
      targetBitmap->frozenCounter = RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN;
      targetBitmap->frozenStorage.data = this->frozenBuffer;
      targetBitmap->frozenStorage.length = std::numeric_limits<size_t>::max();
      this->frozenBuffer = nullptr;
    }
  }
};

class RoaringBitmapDeserializer final : public RoaringBitmapDeserializerBase {
 public:
  RoaringBitmap32 * targetBitmap = nullptr;
  v8utils::TypedArrayContent<uint8_t> inputBuffer;

  WorkerError setOutput(v8::Isolate * isolate, const v8::MaybeLocal<v8::Value> & valueMaybe, DeserializationFormat format) {
    this->isolate = isolate;
    this->format = format;

    if (valueMaybe.IsEmpty()) {
      return WorkerError();
    }

    v8::Local<v8::Value> v;
    if (!valueMaybe.ToLocal(&v) || v->IsNullOrUndefined()) {
      return WorkerError();
    }

    if (!this->inputBuffer.set(isolate, v)) {
      return WorkerError("RoaringBitmap32 deserialization output argument was not a valid typed array");
    }

    return WorkerError();
  }

  WorkerError parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info, bool isInstanceMethod) {
    v8::Isolate * isolate = info.GetIsolate();
    this->isolate = isolate;
    v8::HandleScope scope(isolate);

    if (isInstanceMethod) {
      this->targetBitmap = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
      if (this->targetBitmap == nullptr) {
        return WorkerError("RoaringBitmap32 deserialization on invalid object");
      }
      if (this->targetBitmap->isFrozen()) {
        return WorkerError(ERROR_FROZEN);
      }
    }

    if (info.Length() < 2) {
      return WorkerError("RoaringBitmap32 deserialization expects a format and a buffer arguments");
    }

    int bufferArgIndex = 1;
    DeserializationFormat fmt = tryParseDeserializationFormat(info[0], isolate);
    if (fmt == DeserializationFormat::INVALID) {
      bufferArgIndex = 0;
      fmt = tryParseDeserializationFormat(info[1], isolate);
    }
    this->format = fmt;

    if (
      !info[bufferArgIndex]->IsNullOrUndefined() &&
      !this->inputBuffer.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
      return WorkerError("RoaringBitmap32 deserialization buffer argument was invalid");
    }

    return WorkerError();
  }

  WorkerError deserialize() { return this->deserializeBuf((const char *)this->inputBuffer.data, this->inputBuffer.length); }
};

class RoaringBitmapFileDeserializer final : public RoaringBitmapDeserializerBase {
 public:
  std::string filePath;

  WorkerError parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Isolate * isolate = info.GetIsolate();
    this->isolate = isolate;
    v8::HandleScope scope(isolate);

    if (info.Length() < 2) {
      return WorkerError("RoaringBitmap32::deserializeFileAsync expects a file path and format");
    }

    if (!info[0]->IsString()) {
      return WorkerError("RoaringBitmap32::deserializeFileAsync expects a file path as the first argument");
    }

    v8::String::Utf8Value filePathUtf8(isolate, info[0]);
    this->filePath = std::string(*filePathUtf8, filePathUtf8.length());

    DeserializationFormat fmt = tryParseDeserializationFormat(info[1], isolate);
    if (fmt == DeserializationFormat::INVALID) {
      return WorkerError("RoaringBitmap32::deserializeFileAsync invalid format");
    }
    this->format = fmt;
    return WorkerError();
  }

  WorkerError deserialize() {
    int fd = open(this->filePath.c_str(), O_RDONLY);
    if (fd == -1) {
      return WorkerError::from_errno("open", this->filePath);
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
      WorkerError err = WorkerError::from_errno("fstat", this->filePath);
      close(fd);
      return err;
    }

    size_t fileSize = st.st_size;

    if (fileSize == 0) {
      WorkerError err = this->deserializeBuf(nullptr, 0);
      close(fd);
      return err;
    }

    void * buf = mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
      std::cout << "mmap deserialize failed" << std::endl;

      // mmap failed, try to read the file into a buffer
      buf = gcaware_aligned_malloc(fileSize, 32);
      if (buf != nullptr) {
        ssize_t bytesRead = read(fd, buf, fileSize);
        if (bytesRead == -1) {
          WorkerError err = WorkerError::from_errno("read", this->filePath);
          close(fd);
          gcaware_aligned_free(buf);
          return err;
        }
        if ((size_t)bytesRead != fileSize) {
          WorkerError err = WorkerError("RoaringBitmap32::deserializeFileAsync read less bytes than expected");
          close(fd);
          gcaware_aligned_free(buf);
          return err;
        }
        WorkerError err = this->deserializeBuf((const char *)buf, fileSize);
        gcaware_aligned_free(buf);
        close(fd);
        return err;
      }

      WorkerError err = WorkerError::from_errno("mmap", this->filePath);
      close(fd);
      return err;
    }

    WorkerError err = this->deserializeBuf((const char *)buf, fileSize);

    munmap(buf, fileSize);
    close(fd);
    return err;
  }
};

#endif  // ROARING_NODE_SERIALIZATION_

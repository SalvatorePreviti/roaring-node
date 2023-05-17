#ifndef ROARING_NODE_SERIALIZATION_CSV_
#define ROARING_NODE_SERIALIZATION_CSV_

#include "includes.h"
#include <fcntl.h>
#include "mmap.h"
#include "serialization-format.h"
#include "WorkerError.h"

struct CsvFileDescriptorSerializer final {
 public:
  static int iterate(const roaring::api::roaring_bitmap_t * r, int fd, FileSerializationFormat format) {
    char separator;
    switch (format) {
      case FileSerializationFormat::newline_separated_values: separator = '\n'; break;
      case FileSerializationFormat::comma_separated_values: separator = ','; break;
      case FileSerializationFormat::tab_separated_values: separator = '\t'; break;
      case FileSerializationFormat::json_array: separator = ','; break;
      default: return EINVAL;
    }

    CsvFileDescriptorSerializer writer(fd, separator);
    if (format == FileSerializationFormat::json_array) {
      writer.appendChar('[');
    }

    if (r) {
      roaring_iterate(r, roaringIteratorFn, &writer);
    }

    if (format == FileSerializationFormat::newline_separated_values) {
      writer.appendChar('\n');
    } else if (format == FileSerializationFormat::json_array) {
      writer.appendChar(']');
    }

    if (!writer.flush()) {
      int errorno = errno;
      errno = 0;
      return errorno ? errorno : EIO;
    }

    return 0;
  }

 private:
  const constexpr static size_t BUFFER_SIZE = 131072;

  char * buf;
  size_t bufPos;
  int fd;
  bool needsSeparator;
  char separator;

  CsvFileDescriptorSerializer(int fd, char separator) :
    buf((char *)gcaware_aligned_malloc(32, BUFFER_SIZE)), bufPos(0), fd(fd), needsSeparator(false), separator(separator) {}

  ~CsvFileDescriptorSerializer() { gcaware_aligned_free(this->buf); }

  bool flush() {
    if (this->bufPos == 0) {
      return true;
    }
    if (!this->buf) {
      return false;
    }
    ssize_t written = write(this->fd, this->buf, this->bufPos);
    if (written < 0) {
      gcaware_aligned_free(this->buf);
      this->buf = nullptr;
      return false;
    }
    this->bufPos = 0;
    return true;
  }

  bool appendChar(char c) {
    if (this->bufPos + 1 >= BUFFER_SIZE) {
      if (!this->flush()) {
        return false;
      }
    }
    if (!this->buf) {
      return false;
    }
    this->buf[this->bufPos++] = c;
    return true;
  }

  bool appendValue(uint32_t value) {
    if (this->bufPos + 15 >= BUFFER_SIZE) {
      if (!this->flush()) {
        return false;
      }
    }
    if (!this->buf) {
      return false;
    }
    if (this->needsSeparator) {
      this->buf[this->bufPos++] = this->separator;
    }
    this->needsSeparator = true;

    char * str = this->buf + this->bufPos;
    int32_t i, j;
    char c;

    /* uint to decimal  */
    i = 0;
    do {
      uint32_t remainder = value % 10;
      str[i++] = (char)(remainder + 48);
      value = value / 10;
    } while (value != 0);

    this->bufPos += i;

    /* reverse string */
    for (j = 0, i--; j < i; j++, i--) {
      c = str[i];
      str[i] = str[j];
      str[j] = c;
    }

    return true;
  }

  static bool roaringIteratorFn(uint32_t value, void * param) {
    return ((CsvFileDescriptorSerializer *)param)->appendValue(value);
  }
};

#endif

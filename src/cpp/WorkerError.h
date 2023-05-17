#ifndef ROARING_NODE_WORKER_ERROR_
#define ROARING_NODE_WORKER_ERROR_

#include "includes.h"
#include "v8utils.h"

struct WorkerError {
  const char * msg;
  const char * syscall;
  int errorno;
  std::string path;

  explicit WorkerError() : msg(nullptr), syscall(nullptr), errorno(0) {}

  explicit WorkerError(const char * msg) : msg(msg), syscall(nullptr), errorno(0) {}

  explicit WorkerError(int errorno, const char * syscall, const std::string & path) :
    msg(nullptr), syscall(syscall), errorno(errorno ? errorno : 5), path(path) {}

  inline bool hasError() const { return (msg != nullptr && msg[0] != '\0') || errorno != 0; }

  static WorkerError from_errno(const char * syscall, const std::string & path) {
    int errorno = errno;
    errno = 0;
    return WorkerError(errorno, syscall, path);
  }

  v8::Local<v8::Value> newV8Error(v8::Isolate * isolate) const {
    v8::EscapableHandleScope handle_scope(isolate);
    v8::Local<v8::Value> output;
    if (this->errorno) {
      output = node::ErrnoException(
        isolate, this->errorno, this->syscall, this->msg && this->msg[0] ? this->msg : nullptr, this->path.c_str());
    } else {
      const char * msg = this->msg && this->msg[0] ? this->msg : "Invalid operation";
      v8::MaybeLocal<v8::String> message = v8::String::NewFromUtf8(isolate, msg, v8::NewStringType::kInternalized);
      output = v8::Exception::Error(message.IsEmpty() ? v8::String::Empty(isolate) : message.ToLocalChecked());
    }
    return handle_scope.Escape(output);
  }
};

#endif

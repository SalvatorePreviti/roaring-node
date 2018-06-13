#include "node.h"

// unless DISABLEAVX was defined, if we have __AVX2__, we enable AVX
#if (!defined(USEAVX)) && (!defined(DISABLEAVX)) && (defined(__AVX2__))
#define USEAVX
#endif

#if (defined(__POPCNT__)) && (defined(__SSE4_2__))
#define USESSE4
#endif

#if (defined(USEAVX) || defined(USESSE4))
#define MUST_CHECK_CPUID
#endif

#ifdef MUST_CHECK_CPUID
#ifdef _MSC_VER
#include <intrin.h>
#define __cpuid_count(__level, __count, __eax, __ebx, __ecx, __edx) \
  {                                                                 \
    int info[4];                                                    \
    __cpuidex(info, __level, __count);                              \
    __eax = info[0];                                                \
    __ebx = info[1];                                                \
    __ecx = info[2];                                                \
    __edx = info[3];                                                \
  }
int __get_cpuid_max_func() {
  int info[4];
  __cpuidex(info, 0, 0);
  return info[0];
}
#define __get_cpuid_max(a, b) __get_cpuid_max_func()
#else
#include <cpuid.h>
#endif
#endif

const char * getInstructionsName() {
  const char * result = "";
  unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;

#ifdef MUST_CHECK_CPUID
  auto max_level = __get_cpuid_max(0, NULL);

  bool sse42 = false;
#ifdef USESSE4
  if (max_level >= 1) {
    __cpuid_count(1, 0, eax, ebx, ecx, edx);
    if (ecx & (1 << 20)) {
      sse42 = true;
      result = "sse42";
    }
  }
#endif

#ifdef USEAVX
  if (sse42 && max_level >= 7) {
    __cpuid_count(7, 0, eax, ebx, ecx, edx);
    if (ebx & (1 << 5)) {
      result = "avx2";
    }
  }
#endif
#endif
  return result;
}

void InitModule(v8::Local<v8::Object> exports) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  exports->Set(v8::String::NewFromUtf8(isolate, "cpu"), v8::String::NewFromUtf8(isolate, getInstructionsName()));
}

NODE_MODULE(roaring, InitModule);

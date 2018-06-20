#include "node.h"

#if (!defined(DISABLE_X64)) && (defined(IS_X64) || defined(_M_X64) || defined(__x86_64__) || defined(_M_X64))
#  define USECPUID
#  if ((defined(_MSC_VER)) || (defined(__INTEL_COMPILER)))
#    include <intrin.h>
#  endif
#endif

inline static void _cpuid(unsigned int info[4U], const unsigned int info_type) {
  info[0] = info[1] = info[2] = info[3] = 0;
#ifdef USECPUID
#  ifdef _MSC_VER
  __cpuid((int *)info, info_type);
#  elif defined(__x86_64__)
  __asm__ __volatile__("xchgq %%rbx, %q1; cpuid; xchgq %%rbx, %q1" : "=a"(info[0]), "=&r"(info[1]), "=c"(info[2]), "=d"(info[3]) : "0"(info_type), "2"(0U));
#  else
  __asm__ __volatile__("cpuid" : "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3]) : "0"(info_type), "2"(0U));
#  endif
#endif
}

inline static void getSupportedInstructions(bool & sse42, bool & avx2) {
  unsigned int info[4];
  _cpuid(info, 0x0);

  if (info[0] > 0) {
    _cpuid(info, 0x00000001);
    if ((info[2] & (1 << 20)) && (info[2] & (1 << 23))) {
      sse42 = true;
    }
  }

  if (sse42 && info[0] >= 7) {
    _cpuid(info, 0x00000007);
    avx2 = (info[1] & (1 << 5)) != 0;
  }
}

void InitModule(v8::Local<v8::Object> exports) {
  bool sse42 = false;
  bool avx2 = false;

  getSupportedInstructions(sse42, avx2);

  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  exports->Set(v8::String::NewFromUtf8(isolate, "SSE42"), v8::Boolean::New(isolate, sse42));
  exports->Set(v8::String::NewFromUtf8(isolate, "AVX2"), v8::Boolean::New(isolate, avx2));
}

NODE_MODULE(roaring, InitModule);

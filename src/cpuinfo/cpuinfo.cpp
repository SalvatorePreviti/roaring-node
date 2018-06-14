#include "node.h"

#if (defined(_MSC_VER)) || (defined(__INTEL_COMPILER))
#include <intrin.h>
#endif

void _cpuid(unsigned int info[4U], const unsigned int info_type) {
  info[0] = info[1] = info[2] = info[3] = 0;

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86))
  __cpuid((int *)info, info_type);
#else
#if defined(__i386__)
  __asm__ __volatile__(
      "pushfl; pushfl; "
      "popl %0; "
      "movl %0, %1; xorl %2, %0; "
      "pushl %0; "
      "popfl; pushfl; popl %0; popfl"
      : "=&r"(info[0]), "=&r"(info[1])
      : "i"(0x200000));
  if (((info[0] ^ info[1]) & 0x200000) == 0x0) {
    return; /* LCOV_EXCL_LINE */
  }
#endif
#ifdef __i386__
  __asm__ __volatile__("xchgl %%ebx, %k1; cpuid; xchgl %%ebx, %k1" : "=a"(info[0]), "=&r"(info[1]), "=c"(info[2]), "=d"(info[3]) : "0"(info_type), "2"(0U));
#elif defined(__x86_64__)
  __asm__ __volatile__("xchgq %%rbx, %q1; cpuid; xchgq %%rbx, %q1" : "=a"(info[0]), "=&r"(info[1]), "=c"(info[2]), "=d"(info[3]) : "0"(info_type), "2"(0U));
#else
  __asm__ __volatile__("cpuid" : "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3]) : "0"(info_type), "2"(0U));
#endif
#endif
}

void getSupportedInstructions(bool & sse42, bool & avx2) {
  unsigned int info[4];

  _cpuid(info, 0x0);

  unsigned int nids = info[0];

  if (nids > 0) {
    _cpuid(info, 0x00000001);
    if ((info[2] & (1 << 20)) && (info[2] & (1 << 23))) {
      sse42 = true;
    }
  }

  if (sse42 && nids >= 7) {
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

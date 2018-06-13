#include "node.h"

#if (defined(_MSC_VER)) || (defined(__INTEL_COMPILER))
#include <intrin.h>
void getSupportedInstructions(bool & sse42, bool & avx2) {
  int d[4] = {0, 0, 0, 0};
  __cpuid(d, 1);
  if (((d[0] >> 8) & 15) >= 6) {
    sse42 = (((d[2] & (1 << 20)) != 0) && ((d[2] & (1 << 23)) != 0));
    avx2 = sse42 && (((d[2] & (1 << 28)) != 0) && ((d[2] & (1 << 27)) != 0));
  }
}
#else
#include <cpuid.h>
void getSupportedInstructions(bool & sse42, bool & avx2) {
  sse42 = (__builtin_cpu_supports("sse4.2") && __builtin_cpu_supports("popcnt"));
  avx2 = sse42 && __builtin_cpu_supports("avx2");
}
#endif

void InitModule(v8::Local<v8::Object> exports) {
  bool sse42 = false;
  bool avx2 = false;

  getSupportedInstructions(sse42, avx2);

  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  exports->Set(v8::String::NewFromUtf8(isolate, "SSE42"), v8::Boolean::New(isolate, sse42));
  exports->Set(v8::String::NewFromUtf8(isolate, "AVX2"), v8::Boolean::New(isolate, avx2));
}

NODE_MODULE(roaring, InitModule);

{
  "target_defaults": {
    "default_configuration": "Release"
  },
  "targets": [
    {
      "target_name": "roaring",
      "cflags": ["-O3", "-std=c99"],
      "cflags_cc": ["-O3", "-std=c++11"],
      "include_dirs": ["src", "<!(node -e \"require('nan')\")"],
      "sources": [
        "src/cpp/roaring.c",
        "src/cpp/module.cpp",
        "src/cpp/TypedArrays.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
        "src/cpp/RoaringBitmap32Iterator/RoaringBitmap32Iterator.cpp"
      ]
    }
  ],
  "conditions": [
    [
      "target_arch in \"x64 x86_64\"",
      {
        "targets": [
          {
            "target_name": "cpuinfo",
            "cflags": ["-O3"],
            "cflags_cc": ["-O3"],
            "sources": ["src/cpuinfo/cpuinfo.cpp"]
          },
          {
            "target_name": "roaring-sse42",
            "cflags": ["-O3", "-std=c99", "-msse4.2"],
            "cflags_cc": ["-O3", "-std=c++11", "-msse4.2"],
            "xcode_settings": {
              "GCC_ENABLE_SSE42_EXTENSIONS": "YES",
              "OTHER_CFLAGS": ["-msse4.2"]
            },
            "include_dirs": ["src", "<!(node -e \"require('nan')\")"],
            "sources": [
              "src/cpp/roaring.c",
              "src/cpp/module.cpp",
              "src/cpp/TypedArrays.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
              "src/cpp/RoaringBitmap32Iterator/RoaringBitmap32Iterator.cpp"
            ]
          },
          {
            "target_name": "roaring-avx2",
            "cflags": ["-O3", "-std=c99", "-mavx2"],
            "cflags_cc": ["-O3", "-std=c++11", "-mavx2"],
            "xcode_settings": {
              "GCC_ENABLE_SSE42_EXTENSIONS": "YES",
              "CLANG_X86_VECTOR_INSTRUCTIONS": "avx2",
              "OTHER_CFLAGS": ["-mavx2"]
            },
            "msvs_settings": {
              "VCCLCompilerTool": {
                "AdditionalOptions": ["/arch:AVX2"]
              }
            },
            "include_dirs": ["src", "<!(node -e \"require('nan')\")"],
            "sources": [
              "src/cpp/roaring.c",
              "src/cpp/module.cpp",
              "src/cpp/TypedArrays.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
              "src/cpp/RoaringBitmap32Iterator/RoaringBitmap32Iterator.cpp"
            ]
          }
        ]
      }
    ]
  ]
}

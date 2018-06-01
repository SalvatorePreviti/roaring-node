{
  "targets": [
    {
      "target_name": "roaring",
      "cflags": ["-O3", "-Wall", "-std=c++11"],
      "cflags_cc": ["-O3", "-Wall", "-std=c++11"],
      "include_dirs": ["src", "<!(node -e \"require('nan')\")"],
      "sources": [
        "src/cpp/module.cpp",
        "src/cpp/TypedArrays.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
        "src/cpp/RoaringBitmap32Iterator/RoaringBitmap32Iterator.cpp",
        "src/cpp/roaring.cpp"
      ]
    }
  ]
}

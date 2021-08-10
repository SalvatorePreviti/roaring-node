{
    "targets": [
        {
            "target_name": "roaring",
            "default_configuration": "Release",
            "cflags": ["-O3", "-std=c11", "-fPIC"],
            "cflags_cc": ["-O3", "-std=c++11", "-fPIC"],
            "sources": [
                "src/cpp/roaring.cpp",
                "src/cpp/module.cpp",
                "src/cpp/v8utils/v8utils.cpp",
                "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
                "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
                "src/cpp/RoaringBitmap32BufferedIterator/RoaringBitmap32BufferedIterator.cpp"
            ]
        }
    ]
}

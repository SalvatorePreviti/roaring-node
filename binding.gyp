{
    "targets": [
        {
            "target_name": "roaring",
            "default_configuration": "Release",
            "cflags": ["-O3", "-std=c11"],
            "cflags_cc": ["-O3", "-std=c++latest"],
            "sources": [
                "src/cpp/roaring.cpp",
                "src/cpp/module.cpp",
                "src/cpp/v8utils/v8utils.cpp",
                "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
                "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
                "src/cpp/RoaringBitmap32BufferedIterator/RoaringBitmap32BufferedIterator.cpp"
            ],
            "conditions": [
                [
                    "OS=='win'", {
                        "configurations": {
                            "Release": {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
                                        "AdditionalOptions": ["/std:c++latest", "/Zc:__cplusplus"]
                                    }
                                }
                            },
                            "Debug": {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
                                        "AdditionalOptions": ["/std:c++latest", "/Zc:__cplusplus"]
                                    }
                                }
                            }
                        }
                    }
                ]
            ]
        }
    ]
}

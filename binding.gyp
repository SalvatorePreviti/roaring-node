{
    "targets": [
        {
            "target_name": "roaring",
            "default_configuration": "Release",
            "cflags_cc": ["-O3", "-std=c++11"],
            "sources": [
                "src/cpp/roaring.cpp",
                "src/cpp/module.cpp",
                "src/cpp/v8utils/v8utils.cpp",
                "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
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

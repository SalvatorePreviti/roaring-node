{
    "targets": [
        {
            "target_name": "roaring",
            "default_configuration": "Release",
            "cflags_cc": ["-O3", "-std=c++14"],
            "sources": [
                "src/cpp/v8utils/v8utils.cpp",
                "src/cpp/RoaringBitmap32.cpp",
                "src/cpp/roaring.cpp",
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

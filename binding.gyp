{
    "targets": [
        {
            "target_name": "roaring",
            "default_configuration": "Release",
            "cflags_cc": ["-O3", "-std=c++14", "-mcpu=native", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto"],
            'xcode_settings': {
                'OTHER_CFLAGS': ["-O3", "-std=c++14", "-mcpu=native", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto"],
            },
            "sources": [
                "src/cpp/RoaringBitmap32.cpp"
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

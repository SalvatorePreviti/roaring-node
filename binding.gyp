{
    "variables": {
        "openssl_fips": ''
    },
    "targets": [
        {
            "target_name": "roaring",
            "default_configuration": "Release",
            'include_dirs': ['submodules/CRoaring/include'],
            "cflags_cc": ["-O3", "-std=c++17", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto", "-Wno-unused-variable", "-Wno-cast-function-type"],
            'xcode_settings': {
                'OTHER_CFLAGS': ["-O3", "-std=c++17", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto", "-Wno-unused-function"],
            },
            "sources": [
                "roaring-node.cpp"
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

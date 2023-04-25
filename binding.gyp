{
    "variables": {
        "openssl_fips": ''
    },
    "targets": [
        {
            "target_name": "<(module_name)",
            "product_dir": "<(module_path)",
            "default_configuration": "Release",
            "include_dirs": ["submodules/CRoaring/include"],
            "cflags_cc": ["-O3", "-g0", "-std=c++17", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto", "-Wno-unused-function", "-Wno-unused-variable", "-Wno-cast-function-type"],
            "ldflags": ["-s"],
            "xcode_settings": {
                "GCC_GENERATE_DEBUGGING_SYMBOLS": "NO",
                "OTHER_CFLAGS": ["-O3", "-g0", "-std=c++17", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto", "-Wno-unused-function", "-Wno-unused-variable", "-Wno-cast-function-type"],
            },
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "AdditionalOptions": ["/std:c++latest"]
                }
            },
            "sources": ["roaring-node.cpp"]
        }
    ]
}

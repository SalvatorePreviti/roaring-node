{
    "variables": {
        "openssl_fips": ''
    },
    "targets": [
        {
            "target_name": "<(module_name)",
            "product_dir": "<(module_path)",
            "default_configuration": "Debug",
            "include_dirs": ["submodules/CRoaring/include"],
            "cflags_cc": ["-g", "-std=c++17", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto", "-Wno-unused-function", "-Wno-unused-variable", "-Wno-cast-function-type"],
            "ldflags": ["-s"],
            "xcode_settings": {
                "GCC_GENERATE_DEBUGGING_SYMBOLS": "NO",
                "OTHER_CFLAGS": ["-g", "-std=c++17", "-fno-rtti", "-fno-exceptions", "-fvisibility=hidden", "-flto", "-Wno-unused-function", "-Wno-unused-variable", "-Wno-cast-function-type"],
            },
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "DebugInformationFormat": 0,
                    "Optimization": 3,
                    "AdditionalOptions": ["/O2", "/std:c++latest"]
                }
            },
            "sources": ["roaring-node.cpp"]
        }
    ]
}

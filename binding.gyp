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
            "cflags_cc": [
                "-O3",
                "-g0",
                "-std=c++20",
                "-fno-rtti",
                "-fno-exceptions",
                "-fvisibility=hidden",
                "-flto",
                "-DNDEBUG",
                "-ffunction-sections",
                "-fdata-sections",
                "-fomit-frame-pointer",
                "-fno-unwind-tables",
                "-fno-asynchronous-unwind-tables",
                "-Wno-unused-function",
                "-Wno-unused-variable",
                "-Wno-cast-function-type"
            ],
            "ldflags": ["-s", "-Wl,--gc-sections"],
            "xcode_settings": {
                "GCC_GENERATE_DEBUGGING_SYMBOLS": "NO",
                "OTHER_CFLAGS": [
                    "-O3",
                    "-g0",
                    "-std=c++20",
                    "-fno-rtti",
                    "-fno-exceptions",
                    "-fvisibility=hidden",
                    "-flto",
                    "-DNDEBUG",
                    "-ffunction-sections",
                    "-fdata-sections",
                    "-fomit-frame-pointer",
                    "-fno-unwind-tables",
                    "-fno-asynchronous-unwind-tables",
                    "-Wno-unused-function",
                    "-Wno-unused-variable",
                    "-Wno-cast-function-type"
                ],
                "OTHER_LDFLAGS": ["-Wl,-dead_strip"]
            },
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "DebugInformationFormat": 0,
                    "Optimization": 3,
                    "EnableFunctionLevelLinking": "true",
                    "AdditionalOptions": ["/O2", "/std:c++latest", "/DNDEBUG", "/Gy", "/Gw"]
                },
                "VCLinkerTool": {
                    "OptimizeReferences": "2",
                    "EnableCOMDATFolding": "2"
                }
            },
            "sources": ["roaring-node.cpp"]
        }
    ]
}

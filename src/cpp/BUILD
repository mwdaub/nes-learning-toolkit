package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "nes.so",
    srcs = ["module.cpp", "emulator.h"],
    deps = [
        "//src/cpp/nes:nes_core",
        "@python_headers//:headers",
        "@numpy_headers//:headers",
    ],
    copts = ["-std=c++14", "-Wno-unused-function"],
    linkopts = ["-undefined", "dynamic_lookup"],
    linkshared = True,
)

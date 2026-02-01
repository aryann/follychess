load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "display",
    srcs = [],
    hdrs = ["display.h"],
    visibility = [
        "//cli:__subpackages__",
    ],
    deps = [
        "//cli:command",
        "//engine:game",
        "@abseil-cpp//absl/strings",
    ],
)

cc_library(
    name = "perft_command",
    srcs = [],
    hdrs = ["perft_command.h"],
    visibility = [
        "//cli:__subpackages__",
    ],
    deps = [
        "//cli:command",
        "//engine:perft",
        "//engine:position",
        "@abseil-cpp//absl/strings",
    ],
)

cc_library(
    name = "position_command",
    srcs = ["position_command.cc"],
    hdrs = ["position_command.h"],
    visibility = [
        "//cli:__subpackages__",
    ],
    deps = [
        "//cli:command",
        "//engine:game",
        "//engine:move",
        "//engine:perft",
        "//engine:position",
        "@abseil-cpp//absl/strings",
    ],
)

cc_library(
    name = "isready_command",
    srcs = [],
    hdrs = ["isready_command.h"],
    visibility = [
        "//cli:__subpackages__",
    ],
    deps = [
        "//cli:command",
    ],
)

cc_library(
    name = "uci_command",
    srcs = [],
    hdrs = ["uci_command.h"],
    visibility = [
        "//cli:__subpackages__",
    ],
    deps = [
        "//cli:command",
        "//cli:options",
        "//engine:game",
        "//search",
    ],
)

# BUILD file for fastchess (https://github.com/Disservin/fastchess), which
# does not ship Bazel support. The source list mirrors app/Makefile:
#
#   * All sources under app/src except main.cpp (which lives in the cc_binary)
#     and engine/process/test.cpp (a test helper that is not part of the
#     build).
#   * third_party/pyrrhic/tbprobe.cpp, which textually includes tbchess.c.
#   * The vendored fmt is used in header-only mode.
#   * The gzip module (zlib support) is off, matching the Makefile default.
#
# fastchess is C++17, so -std=c++17 overrides this project's C++23 default
# for these targets.

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

FASTCHESS_COPTS = ["-std=c++17"]

# RELEASE matches upstream's release builds: it makes the version string skip
# the __DATE__-based build stamp, which crashes under Bazel because Bazel
# redefines __DATE__ to "redacted" for reproducibility.
#
# posix_spawn_file_actions_addchdir_np is available on macOS 10.15+; the
# upstream Makefile detects this with a compile check
# (app/configuration/spawn-check.mk).
FASTCHESS_LOCAL_DEFINES = ["RELEASE"] + select({
    "@platforms//os:osx": ["HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR_NP"],
    "//conditions:default": [],
})

cc_library(
    name = "fastchess_lib",
    srcs = glob(
        ["app/src/**/*.cpp"],
        exclude = [
            "app/src/main.cpp",
            "app/src/engine/process/test.cpp",
        ],
    ) + ["app/third_party/pyrrhic/tbprobe.cpp"],
    hdrs = glob(
        [
            "app/src/**/*.h",
            "app/src/**/*.hpp",
            "app/third_party/**/*.h",
            "app/third_party/**/*.hpp",
        ],
        exclude = ["app/third_party/fmt/test/**"],
    ),
    copts = FASTCHESS_COPTS,
    includes = [
        "app/src",
        "app/third_party",
    ],
    linkopts = ["-pthread"],
    local_defines = FASTCHESS_LOCAL_DEFINES,
    textual_hdrs = ["app/third_party/pyrrhic/tbchess.c"],
)

cc_binary(
    name = "fastchess",
    srcs = ["app/src/main.cpp"],
    copts = FASTCHESS_COPTS,
    local_defines = FASTCHESS_LOCAL_DEFINES,
    visibility = ["//visibility:public"],
    deps = [":fastchess_lib"],
)

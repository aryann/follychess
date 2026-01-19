package(
    default_visibility = [
        "//:__subpackages__",
    ],
)

cc_library(
    name = "evaluation",
    srcs = ["evaluation.cc"],
    hdrs = ["evaluation.h"],
    deps = [
        ":phase",
        "//engine:attacks",
        "//engine:move",
        "//engine:move_generator",
        "//engine:position",
        "//engine:scoped_move",
        "//engine:types",
    ],
)

cc_test(
    name = "evaluation_test",
    srcs = ["evaluation_test.cc"],
    deps = [
        ":evaluation",
        "//engine:position",
        "//engine:testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "history_heuristic",
    srcs = [],
    hdrs = ["history_heuristic.h"],
    deps = [
        "//engine:move",
    ],
)

cc_library(
    name = "killer_moves",
    hdrs = ["killer_moves.h"],
    deps = [
        "//engine:move",
    ],
)

cc_test(
    name = "killer_moves_test",
    srcs = ["killer_moves_test.cc"],
    deps = [
        ":killer_moves",
        "//engine:move",
        "//engine:testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "move_ordering",
    srcs = ["move_ordering.cc"],
    hdrs = ["move_ordering.h"],
    deps = [
        ":history_heuristic",
        ":killer_moves",
        "//engine:move",
        "//engine:position",
    ],
)

cc_test(
    name = "move_ordering_test",
    srcs = ["move_ordering_test.cc"],
    deps = [
        ":move_ordering",
        "//engine:move",
        "//engine:position",
        "//engine:testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "phase",
    srcs = ["phase.cc"],
    hdrs = ["phase.h"],
    deps = [
        "//engine:position",
    ],
)

cc_test(
    name = "phase_test",
    srcs = ["phase_test.cc"],
    deps = [
        ":phase",
        "//engine:position",
        "//engine:testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "principal_variation",
    srcs = [],
    hdrs = ["principal_variation.h"],
    deps = [
        "//engine:move",
    ],
)

cc_test(
    name = "principal_variation_test",
    srcs = ["principal_variation_test.cc"],
    deps = [
        ":principal_variation",
        "//engine:move",
        "//engine:position",
        "//engine:testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "search",
    srcs = ["search.cc"],
    hdrs = ["search.h"],
    deps = [
        ":evaluation",
        ":history_heuristic",
        ":killer_moves",
        ":move_ordering",
        ":principal_variation",
        ":transposition",
        "//engine:move",
        "//engine:move_generator",
        "//engine:position",
        "//engine:scoped_move",
        "//engine:types",
    ],
)

cc_test(
    name = "search_test",
    srcs = ["search_test.cc"],
    deps = [
        ":search",
        "//engine:move",
        "//engine:move_generator",
        "//engine:position",
        "//engine:scoped_move",
        "//engine:testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "transposition",
    srcs = [],
    hdrs = ["transposition.h"],
    deps = [
        ":evaluation",
        "//engine:position",
        "@abseil-cpp//absl/container:flat_hash_map",
    ],
)

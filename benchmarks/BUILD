cc_binary(
    name = "attacks_benchmark",
    srcs = ["attacks_benchmark.cc"],
    deps = [
        "//engine:attacks",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@google_benchmark//:benchmark",
    ],
)

cc_binary(
    name = "generate_moves_benchmark",
    srcs = ["generate_moves_benchmark.cc"],
    deps = [
        "//engine:move_generator",
        "//engine:position",
        "//engine:scoped_move",
        "//engine:types",
        "@google_benchmark//:benchmark",
    ],
)

cc_binary(
    name = "moves_benchmark",
    srcs = ["moves_benchmark.cc"],
    deps = [
        "//engine:move_generator",
        "//engine:position",
        "//engine:scoped_move",
        "//engine:types",
        "@google_benchmark//:benchmark",
    ],
)

cc_binary(
    name = "search_benchmark",
    srcs = ["search_benchmark.cc"],
    deps = [
        "//engine:position",
        "//search",
        "@google_benchmark//:benchmark",
    ],
)

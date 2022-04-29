load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "db_proto",
    srcs = ["db.proto"],
)

cc_proto_library(
    name = "db_proto_cc",
    deps = [":db_proto"],
)

cc_binary(
    name = "goggle",
    srcs = ["goggle.cc"],
    data = [
        "//data",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":db_proto_cc",
        ":doc",
        ":escape_json",
        "//src/extractor",
        "//src/forward_index",
        "//src/trigram",
        "@com_github_google_glog//:glog",
        "@com_github_google_leveldb//:leveldb",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@httplib",
    ],
)

cc_library(
    name = "doc",
    srcs = ["doc.cc"],
    hdrs = ["doc.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "doc_test",
    size = "small",
    srcs = ["doc_test.cc"],
    deps = [
        ":doc",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "escape_json",
    hdrs = ["escape_json.h"],
    visibility = ["//visibility:public"],
)

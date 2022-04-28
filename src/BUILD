load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "pagerank_proto",
    srcs = ["pagerank.proto"],
)

cc_proto_library(
    name = "pagerank_proto_cc",
    deps = [":pagerank_proto"],
)

cc_binary(
    name = "server",
    srcs = ["server.cc"],
    data = [
        "//data",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":doc",
        ":escape_json",
        ":pagerank_proto_cc",
        "//src/extractor",
        "//src/forward_index",
        "//src/trigram",
        "@com_github_google_glog//:glog",
        "@com_github_google_leveldb//:leveldb",
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

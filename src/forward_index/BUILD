load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "doc_proto",
    srcs = ["doc.proto"],
)

cc_proto_library(
    name = "doc_proto_cc",
    deps = [":doc_proto"],
)

cc_library(
    name = "forward_index",
    srcs = ["forward_index.cc"],
    hdrs = ["forward_index.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":doc_proto_cc",
        "//src:doc",
        "@com_github_google_glog//:glog",
        "@com_github_google_leveldb//:leveldb",
    ],
)

cc_test(
    name = "forward_index_test",
    size = "small",
    srcs = ["forward_index_test.cc"],
    deps = [
        ":forward_index",
        "@com_github_google_glog//:glog",
        "@com_google_googletest//:gtest_main",
    ],
)

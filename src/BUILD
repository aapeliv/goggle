load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "common",
    hdrs = ["common.h"],
    visibility = ["//visibility:public"],
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
        "//src/extractor",
        "//src/forward_index",
        "//src/trigram",
        "@com_github_google_glog//:glog",
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

cc_binary(
    name = "leveldb_test",
    srcs = ["leveldb_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_github_google_leveldb//:leveldb",
    ],
)

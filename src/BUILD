load("@rules_cc//cc:defs.bzl", "cc_library")

cc_binary(
    name = "server",
    srcs = ["server.cc"],
    visibility = ["//visibility:public"],
    deps = [
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

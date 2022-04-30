load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "trigram_proto",
    srcs = ["trigram.proto"],
)

cc_proto_library(
    name = "trigram_proto_cc",
    deps = [":trigram_proto"],
)

cc_library(
    name = "trigram",
    srcs = ["trigram.cc"],
    hdrs = ["trigram.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":trigram_proto_cc",
        ":trigram_splitter",
        "@com_github_google_glog//:glog",
        "@com_github_google_leveldb//:leveldb",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "trigram_test",
    size = "small",
    srcs = ["trigram_test.cc"],
    deps = [
        ":trigram",
        "@com_github_google_glog//:glog",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "trigram_splitter",
    srcs = ["trigram_splitter.cc"],
    hdrs = ["trigram_splitter.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_test(
    name = "trigram_splitter_test",
    size = "small",
    srcs = ["trigram_splitter_test.cc"],
    deps = [
        ":trigram_splitter",
        "@com_github_google_glog//:glog",
        "@com_google_googletest//:gtest_main",
    ],
)

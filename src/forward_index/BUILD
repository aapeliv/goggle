cc_library(
    name = "forward_index",
    srcs = ["forward_index.cc"],
    hdrs = ["forward_index.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src:common",
        "//src:doc",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/container:flat_hash_map",
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

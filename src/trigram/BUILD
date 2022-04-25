cc_library(
    name = "trigram",
    hdrs = ["trigram.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":trigram_splitter",
        "@com_github_google_glog//:glog",
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

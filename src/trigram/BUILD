cc_library(
    name = "trigram",
    hdrs = ["trigram.h"],
    deps = [],
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

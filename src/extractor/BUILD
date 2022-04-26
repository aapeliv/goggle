cc_library(
    name = "extractor",
    srcs = ["extractor.cc"],
    hdrs = ["extractor.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src:doc",
        "//third_party/bzip2",
        "//third_party/rapidxml",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_test(
    name = "extractor_test",
    size = "medium",
    srcs = ["extractor_test.cc"],
    data = [
        "//data",
    ],
    deps = [
        ":extractor",
        "@com_google_googletest//:gtest_main",
    ],
)

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
        "@httplib",
    ],
)

cc_test(
    name = "extractor_test",
    size = "medium",
    srcs = ["extractor_test.cc"],
    data = [
        "enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242.bz2",
        "enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2",
    ],
    deps = [
        ":extractor",
        "@com_google_googletest//:gtest_main",
    ],
)

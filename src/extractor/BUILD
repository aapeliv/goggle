cc_binary(
    name = "extractor",
    srcs = ["extractor.cc"],
    data = ["enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2"],
    visibility = ["//visibility:public"],
    deps = [
        "//src:doc",
        "//third_party/bzip2",
        "//third_party/rapidxml",
        "@com_github_google_glog//:glog",
        "@httplib",
    ],
)

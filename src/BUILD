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

cc_binary(
    name = "doc",
    srcs = ["doc.cc"],
    visibility = ["//visibility:public"],
)

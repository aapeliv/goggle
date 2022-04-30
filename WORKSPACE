workspace(name = "next")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# abseil

http_archive(
    name = "com_google_absl",
    sha256 = "3943578b8cb4a356717ee1e9069f367e149e07792fbd6e2910415c55204eff30",
    strip_prefix = "abseil-cpp-5e4ea1ce097f3571e7d87af33b6b30d11b3a211e",
    urls = ["https://github.com/abseil/abseil-cpp/archive/5e4ea1ce097f3571e7d87af33b6b30d11b3a211e.zip"],
)

# gflags

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

# glog

http_archive(
    name = "com_github_google_glog",
    sha256 = "9d8ddfd88d579bcc8c6f71595470d5ad590626366b8da6fb4e272eb9a9468872",
    strip_prefix = "glog-a8e0007e96ff96145022c488e367da10f835c75d",
    urls = ["https://github.com/google/glog/archive/a8e0007e96ff96145022c488e367da10f835c75d.zip"],
)

# googletest

http_archive(
    name = "com_google_googletest",
    sha256 = "1cff5915c9dfbf8241d811e95230833c4f34a6d56b7b8c960f4c828f60429a38",
    strip_prefix = "googletest-c9461a9b55ba954df0489bab6420eb297bed846b",
    urls = ["https://github.com/google/googletest/archive/c9461a9b55ba954df0489bab6420eb297bed846b.zip"],
)

# google benchmark

http_archive(
    name = "com_github_google_benchmark",
    sha256 = "e0902204d0ac98461aae8fea48082e3942d7de4630dc579b1d0b41e4fd2c22e6",
    strip_prefix = "benchmark-7cb2914292886a1c2625faf5cc84743d1855d21b",
    urls = ["https://github.com/google/benchmark/archive/7cb2914292886a1c2625faf5cc84743d1855d21b.zip"],
)

# foreign cc rules

http_archive(
    name = "rules_foreign_cc",
    sha256 = "94f8d8fee28baa1827adc8ae21c35d32bb29c78b3b8aca3cbab13a7aaac1a523",
    strip_prefix = "rules_foreign_cc-c57b55f2f6207c0c44c2b4a51dc235955d4f47b2",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/c57b55f2f6207c0c44c2b4a51dc235955d4f47b2.zip",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

# leveldb

http_archive(
    name = "com_github_google_leveldb",
    build_file = "//:third_party/leveldb.BUILD",
    sha256 = "94c424e4de3271741eee932d6c7eb1b6382bab5602935d384f2eae82edc57836",
    strip_prefix = "leveldb-4fb146810cd265ffefa7f9905c016ae965ad36c8",
    urls = ["https://github.com/google/leveldb/archive/4fb146810cd265ffefa7f9905c016ae965ad36c8.zip"],
)

# protobuf

http_archive(
    name = "rules_proto",
    sha256 = "857efa238d7952ba503620d68e39869e9ced90e41d013e6b61da398503f38e08",
    strip_prefix = "rules_proto-3212323502e21b819ac4fbdd455cb227ad0f6394",
    urls = ["https://github.com/bazelbuild/rules_proto/archive/3212323502e21b819ac4fbdd455cb227ad0f6394.zip"],
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

# openssl

http_archive(
    name = "openssl",
    build_file = "//:third_party/openssl.BUILD",
    sha256 = "22d6588e4a7c5ad48fcac2fbf1d035bd43258c22a49457dad0539ded0651b4d2",
    strip_prefix = "openssl-OpenSSL_1_1_1j",
    url = "https://github.com/openssl/openssl/archive/OpenSSL_1_1_1j.tar.gz",
)

# cpp-httplib

http_archive(
    name = "httplib",
    build_file = "//:third_party/httplib.BUILD",
    sha256 = "7f9a8670719cd4b66e70bd1f8af85b6d772ed2fac3d2e528a1bcc494d10b4fb2",
    strip_prefix = "cpp-httplib-d73395e1dc652465fa9524266cd26ad57365491f",
    urls = ["https://github.com/yhirose/cpp-httplib/archive/d73395e1dc652465fa9524266cd26ad57365491f.zip"],
)

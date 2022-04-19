workspace(name = "next")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# protobuf

http_archive(
    name = "rules_proto",
    sha256 = "ec3692776b2973954e22ed3b6ed346e3c43e67269eacc8cd43c7fc6aa47b710a",
    strip_prefix = "rules_proto-0bc8db13f8dc8f5d435be07e276712db1ca11a91",
    urls = ["https://github.com/bazelbuild/rules_proto/archive/0bc8db13f8dc8f5d435be07e276712db1ca11a91.zip"],
)

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

# cpp-httplib

http_archive(
    name = "httplib",
    build_file = "//:third_party/httplib.BUILD",
    sha256 = "7f9a8670719cd4b66e70bd1f8af85b6d772ed2fac3d2e528a1bcc494d10b4fb2",
    strip_prefix = "cpp-httplib-d73395e1dc652465fa9524266cd26ad57365491f",
    urls = ["https://github.com/yhirose/cpp-httplib/archive/d73395e1dc652465fa9524266cd26ad57365491f.zip"],
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

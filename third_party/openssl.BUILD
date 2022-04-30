load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")

# An openssl build file based on a snippet found in the github issue:
# https://github.com/bazelbuild/rules_foreign_cc/issues/337

# Read https://wiki.openssl.org/index.php/Compilation_and_Installation

filegroup(
    name = "all",
    srcs = glob(["**"]),
)

# OpenSSL build rules
# https://github.com/bazelbuild/rules_foreign_cc/issues/337#issuecomment-657004174
CONFIGURE_OPTIONS = [
    "no-weak-ssl-ciphers",
    "no-idea",
    "no-comp",
]

configure_make(
    name = "openssl",
    lib_source = ":all",
    configure_command = "config",
    env = select({
        "@platforms//os:macos": {
            "AR": "",
        },
        "//conditions:default": {},
    }),
    configure_options = select({
        "@platforms//os:macos": [
            "shared",
            "no-afalgeng",
            "ARFLAGS=r",
        ] + CONFIGURE_OPTIONS,
        "//conditions:default": [
        ] + CONFIGURE_OPTIONS,
    }),
    args = ["-j6"],
    out_shared_libs = select({
        "@platforms//os:macos": [
            "libssl.dylib",
            "libssl.1.1.dylib",
            "libcrypto.dylib",
            "libcrypto.1.1.dylib",
        ],
        "//conditions:default": [
            "libssl.so",
            "libssl.so.1.1",
            "libcrypto.so",
            "libcrypto.so.1.1",
        ],
    }),
    visibility = ["//visibility:public"],
)

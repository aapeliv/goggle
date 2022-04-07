# The Wikipedia Goggle

We use [`bazel`](https://bazel.build) as the build system.

## Running tests

```sh
bazel test //...
```

## Running a given executable

```sh
# bazel run //directory:target
# e.g.
bazel run //src:server
# target comes from BUILD file `name` attribute
```

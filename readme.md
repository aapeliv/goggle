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
bazel run //src:google
# target comes from BUILD file `name` attribute
```

## Downloading sample data

```sh
wget https://dumps.wikimedia.org/enwiki/20220401/enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2
```

## Other stuff

As of writing, the max Wikipedia page ID is 70512756 ~ 2^26, so that fits nicely in 4 bytes, and we don't need to map it to an internal ID.

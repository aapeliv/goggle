![The Wikipedia Goggle logo](goggle_text.png)

# The Wikipedia Goggle

The Wikipedia Goggle is a search engine for the English Wikipedia, using a trigram index and a ranking algorithm similar to Google's original PageRank, implemented in modern C++.

## Getting started

1. Install Bazel from <https://bazel.build/install> (you can do `brew install bazel` if you have Homebrew)

2. Clone the git repo

```sh
git clone https://github.com/aapeliv/goggle.git
```

3. Download files from the Wikipedia data dump using. E.g. to get a sample dump from April 1st 2022, go to <https://dumps.wikimedia.org/enwiki/20220401/>, download a partial dump and extract the index file.

```sh
cd data

# download the data dump
wget https://dumps.wikimedia.org/enwiki/20220401/enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2

# download the data index file
wget https://dumps.wikimedia.org/enwiki/20220401/enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242.bz2

# extract the index
bunzip2 enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242.bz2
```

4. Run the tests

```sh
bazel test //...
```

5. Start the indexer and backend with

```sh
bazel run //src:goggle
```

6. Once the backend comes up with a message `Serving on 8080.', you can test it with a query such as

```sh
curl "http://localhost:8080/query?q=finland"
```

## Running the whole thing self contained

1. Build an optimized binary

```sh
bazel build --config=optz //src:goggle`
```

2. Build an optimized frontend

```sh
cd frontend/
npm run build
```

3. Get a TLS certificate and place them in the working directory

4. Download the full Wikipedia dump and index

5. Run the full thing

```sh
./bazel-bin/src/goggle \
  --db_dir=prod_db/ \
  --dump_file path/to/articles-multistream.xml.bz2 \
  --index_file path/to/articles-multistream-index.txt \
  --enable_tls \
  --server_cert path/to/cert.pem \
  --server_key path/to/key.pem \
  --frontend_server_dir frontend/build/
```

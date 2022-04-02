find src -iname '*.h' -o -iname '*.cc' | xargs clang-format -style=file -i
buildifier -r .

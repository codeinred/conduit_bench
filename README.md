# conduit_bench is a set of benchmarks for the conduit library

This repository is a set of benchmarks for the conduit library. It uses google benchmark,
and it compares the conduit library against cppcoro.

## Dependencies

**conduit** can be built with either gcc 10 or clang 10, as long as you have either a modern version of either standard library on your system. **cppcoro**, however, requires clang 10 with libc++, and can't currently be built using gcc.

Because **cppcoro** is the de-facto standard, this benchmark compares **conduit** to **cppcoro**, and so it has to be built with clang 10 and libc++. If your system doesn't come with clang 10 and libc++ as a package, [you can download a binary here](https://releases.llvm.org/download.html).

## Building conduit_bench

In order to build, run:
```bash
git clone --depth=1 --recursive https://github.com/functionalperez/conduit_bench.git

cd conduit_bench
mkdir build && cd build

cmake .. -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DBENCHMARK_ENABLE_GTEST_TESTS=OFF \
  -DCMAKE_CXX_FLAGS="-stdlib=libc++"
```
Finally, run either
```bash
cmake --build . --config Release --parallel
```
Or, if you're using an older version of cmake that doesn't support `--parallel`,
```bash
cmake --build . --config Release
```

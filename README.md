# conduit_bench is a set of benchmarks for the conduit library

This repository is a set of benchmarks for the conduit library. It uses google benchmark,
and it compares the conduit library against cppcoro.

## Dependencies

**conduit** can be built with either gcc 10 or clang 10, as long as you have either a modern version of either standard library on your system. **cppcoro**, however, requires clang 10 with libc++, and can't currently be built using gcc.

Because **cppcoro** is the de-facto standard, this benchmark compares **conduit** to **cppcoro**, and so it has to be built with clang 10 and libc++. If your system doesn't come with clang 10 and libc++ as a package, [you can download a binary here](https://releases.llvm.org/download.html).

**conduit**, **cppcoro**, and **google benchmark** are included as git submodules in the project, and as long as you have clang-10 and libc++, you can clone the project recursively and you don't need to install anything.

## Building conduit_bench

In order to build, run:
```bash
git clone --depth=1 --recursive https://github.com/functionalperez/conduit_bench.git

cd conduit_bench
mkdir build && cd build

cmake .. -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS="-stdlib=libc++"

cmake --build . --config Release

# Run benchmark
./run_benchmark
```

This will build the project and run the benchmark with google benchmark's default settings. For a full list of benchmarking options, run `./conduit_bench --help`. 

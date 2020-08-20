# conduit_bench is a set of benchmarks for the conduit library

This repository is a set of benchmarks for the conduit library. It uses google benchmark,
and it compares the conduit library against cppcoro.

#building conduit_bench

The cppcoro library requires both clang and libc++ in order to build. I've also added
google benchmark as a dependency, because if you have a system installation google benchmark that's built with gcc's 
stdlib instead of libc++, there may be errors when linking. 

The conduit library, on the other hand, can be built with either clang or gcc using
either one's standard library implementation.


In order to build, do:
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
cmake --build . --parallel
```
Or
```bash
cmake --build .
```
(Some older versions of cmake might not support the --parallel option.)

#include <benchmark/benchmark.h>
#include <functional>

template <class Gen>
auto nums() -> Gen {
    long i = 0;
    for (;; i++) {
        co_yield i;
    }
}

extern "C" void baseline(benchmark::State& state) {
    long min = 0;
    long inc = 1;
    for (auto _ : state) {
        min += inc;
        long value = min;
        benchmark::DoNotOptimize(value);
    }
}

extern "C" void std_function(benchmark::State& state) {
    auto func = std::function<long()>([i = 0l]() mutable { return i++; });

    for (auto _ : state) {
        long value = func();
        benchmark::DoNotOptimize(value);
    }
}

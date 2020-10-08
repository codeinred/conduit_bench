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

// Marked volatile so that the compiler can't optimize it
long (*volatile increment_function)(long) = +[](long value) {
    return value + 1;
};

extern "C" void function_pointer(benchmark::State& state) {
    const auto func = increment_function;

    long i = 0;
    for (auto _ : state) {
        i = increment_function(i);
        long value = i;
        benchmark::DoNotOptimize(value);
    }
}

extern "C" void opt_function_pointer(benchmark::State& state) {
    const auto func = increment_function;

    long i = 0;
    long value = 0;
    for (auto _ : state) {
        i = increment_function(i);
        value = i;
    }
    benchmark::DoNotOptimize(value);
}

extern "C" void opt_std_function(benchmark::State& state) {
    auto func = std::function<long()>([i = 0l]() mutable { return i++; });

    long value = 0;
    for (auto _ : state) {
        value = func();
    }
    benchmark::DoNotOptimize(value);
}

#include <benchmark/benchmark.h>
#include <conduit/co_void.hpp>
#include <conduit/future.hpp>
#include <conduit/generator.hpp>
#include <conduit/source.hpp>

static void count_baseline(benchmark::State& state) {
    long min = 0;
    long inc = 1;
    for (auto _ : state) {
        min += inc;
        long value = min;
        benchmark::DoNotOptimize(value);
    }
} 
BENCHMARK(count_baseline);

auto count_generator(long min, long inc) -> conduit::generator<long> {
    for (;; min += inc) {
        co_yield min;
    }
}
static void count_with_generator(benchmark::State& state) {
    auto source = count_generator(0, 1);
    auto it = begin(source);
    for (auto _ : state) {
        auto value = *it;
        ++it;
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(count_with_generator);

auto count_source(long min, long inc) -> conduit::source<long> {
    for (;; min += inc) {
        co_yield min;
    }
}
static void count_with_source(benchmark::State& state) {
    [&]() -> conduit::co_void {
        auto source = count_source(0, 1);
        for (auto _ : state) {
            auto value = *co_await source;
            benchmark::DoNotOptimize(value);
        }
        co_return;
    }();
}
BENCHMARK(count_with_source);

auto get_value_async(long value) -> conduit::future<long> { co_return value; }
static void BM_Future(benchmark::State& state) {
    [&]() -> conduit::co_void {
        for (auto _ : state) {
            auto value = *co_await get_value_async(10);
            benchmark::DoNotOptimize(value);
        }
        co_return;
    }();
}
BENCHMARK(BM_Future);

BENCHMARK_MAIN();
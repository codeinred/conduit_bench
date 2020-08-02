#include <benchmark/benchmark.h>
#include <conduit/co_void.hpp>
#include <conduit/future.hpp>
#include <conduit/generator.hpp>
#include <conduit/source.hpp>

static void BM_count(benchmark::State& state) {
    long min = 0;
    long inc = 1;
    for(auto _ : state) {
        min += inc;
        long value = min;
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(BM_count);

auto count_generator(long min, long inc) -> conduit::generator<long> {
    for (;; min += inc) {
        co_yield min;
    }
}
static void BM_generator(benchmark::State& state) {
    auto source = count_generator(0, 1);
    auto it = begin(source);
    for (auto _ : state) {
        auto value = *it;
        ++it;
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(BM_generator);

auto count_async(long min, long inc) -> conduit::source<long> {
    for (;; min += inc) {
        co_yield min;
    }
}
static void BM_Source(benchmark::State& state) {
    [&]() -> conduit::co_void {
        auto source = count_async(0, 1);
        for (auto _ : state) {
            auto value = co_await source;
            benchmark::DoNotOptimize(value);
        }
        co_return;
    }();
}
BENCHMARK(BM_Source);

auto get_value_async(int value) -> conduit::future<int> { co_return value; }
static void BM_Future(benchmark::State& state) {
    [&]() -> conduit::co_void {
        for (auto _ : state) {
            auto value = co_await get_value_async(10);
            benchmark::DoNotOptimize(value);
        }
        co_return;
    }();
}
BENCHMARK(BM_Future);

BENCHMARK_MAIN();
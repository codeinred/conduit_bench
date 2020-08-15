#include <benchmark/benchmark.h>
#include <conduit/co_void.hpp>
#include <conduit/future.hpp>
#include <conduit/generator.hpp>
#include <conduit/source.hpp>
#include <memory>
#include <functional>

#include <cppcoro/async_generator.hpp>
#include <cppcoro/generator.hpp>

static void count_baseline(benchmark::State& state) {
    long min = 0;
    long inc = 1;
    for (auto _ : state) {
        min += inc;
        long value = min;
        benchmark::DoNotOptimize(value);
    }
}
static void count_std_function(benchmark::State& state) {
    auto func = std::function<long()>([i = 0l] () mutable { return i++; });

    for(auto _ : state) {
        long value = func();
        benchmark::DoNotOptimize(value);
    }
}

template <class Gen>
auto nums() -> Gen {
    long i = 0;
    for (;; i++) {
        co_yield i;
    }
}


static void sync(benchmark::State& state, auto gen) {
    using std::begin;
    auto it = begin(gen);
    for (auto _ : state) {
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}
static auto async(benchmark::State& state, conduit::awaitable auto gen) -> conduit::co_void {
    for(auto _ : state) {
        long value = *co_await gen;
        benchmark::DoNotOptimize(value);
    }
}
static auto async(benchmark::State& state, auto gen) -> conduit::co_void {
    using std::begin;
    auto it = begin(gen);
    for(auto _ : state) {
        long value = *co_await it;
        benchmark::DoNotOptimize(value);
    }
}

static auto async10(benchmark::State& state, conduit::awaitable auto gen) -> conduit::co_void {
    for(auto _ : state) {
        std::array<long, 10> values;
        for(long& v : values) {
            v = *co_await gen;
        }
        benchmark::DoNotOptimize(values);
    }
}
static auto async10(benchmark::State& state, auto gen) -> conduit::co_void {
    using std::begin;
    auto it = begin(gen);
    for(auto _ : state) {
        std::array<long, 10> values;
        for(long& v : values) {
            v = *co_await it;
        }
        benchmark::DoNotOptimize(values);
    }
}
BENCHMARK(count_std_function);
BENCHMARK_CAPTURE(sync, "benchmark conduit::generator", nums<conduit::generator<long>>());
BENCHMARK_CAPTURE(async, "benchmark conduit::source", nums<conduit::source<long>>());
BENCHMARK_CAPTURE(async, "benchmark cppcoro::async_generator", nums<cppcoro::async_generator<long>>());
BENCHMARK_CAPTURE(async10, "benchmark conduit::source x5", nums<conduit::source<long>>());
BENCHMARK_CAPTURE(async10, "benchmark cppcoro::async_generator x5", nums<cppcoro::async_generator<long>>());


BENCHMARK_MAIN();

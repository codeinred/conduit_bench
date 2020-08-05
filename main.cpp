#include <benchmark/benchmark.h>
#include <conduit/co_void.hpp>
#include <conduit/future.hpp>
#include <conduit/generator.hpp>
#include <conduit/source.hpp>

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

template <class Gen>
auto count_coro(long min, long inc) -> Gen {
    for (;; min += inc) {
        co_yield min;
    }
}

static void count_with_conduit_generator(benchmark::State& state) {
    auto source = count_coro<conduit::generator<long>>(0, 1);
    auto it = begin(source);
    for (auto _ : state) {
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}

static void count_with_conduit_async_generator(benchmark::State& state) {
    [&]() -> conduit::co_void {
        auto source = count_coro<conduit::source<long>>(0, 1);
        for (auto _ : state) {
            auto value = *co_await source;
            benchmark::DoNotOptimize(value);
        }
        co_return;
    }();
}

static void count_with_cppcoro_generator(benchmark::State& state) {
    auto source = count_coro<cppcoro::generator<long>>(0, 1);
    using std::begin;
    auto it = begin(source);
    for (auto _ : state) {
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}

auto count_with_cppcoro_async_generator(benchmark::State& state) {
    [&]() -> conduit::co_void {
        auto gen = count_coro<cppcoro::async_generator<long>>(0, 1);
        auto it = gen.begin();
        for (auto _ : state) {
            auto iter = co_await it;
            auto value = *iter;
            benchmark::DoNotOptimize(value);
        }
        co_return;
    }();
}
BENCHMARK(count_baseline);
BENCHMARK(count_with_cppcoro_generator);
BENCHMARK(count_with_conduit_generator);
BENCHMARK(count_with_cppcoro_generator);
BENCHMARK(count_with_conduit_generator);
BENCHMARK(count_with_conduit_async_generator);
BENCHMARK(count_with_cppcoro_async_generator);

BENCHMARK_MAIN();
#include <array>
#include <benchmark/benchmark.h>
#include <conduit/coroutine.hpp>
#include <cppcoro/async_generator.hpp>
#include <cppcoro/generator.hpp>
#include <functional>

template <class Gen>
auto nums() -> Gen {
    long i = 0;
    for (;; i++) {
        co_yield i;
    }
}

extern "C" void generator(benchmark::State& state) {
    using std::begin;
    auto gen = nums<cppcoro::generator<long>>();
    auto it = begin(gen);
    for (auto _ : state) {
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}

extern "C" void async_generator(benchmark::State& state) {
    [&]() -> conduit::coroutine {
        auto gen = nums<cppcoro::async_generator<long>>();
        auto it = std::begin(gen);
        for (auto _ : state) {
            long value = *co_await it;
            benchmark::DoNotOptimize(value);
        }
    }();
}

extern "C" void opt_generator(benchmark::State& state) {
    using std::begin;
    auto gen = nums<cppcoro::generator<long>>();
    auto it = begin(gen);
    long value = 0;
    for (auto _ : state) {
        value = *it;
        it++;
    }
    benchmark::DoNotOptimize(value);
}

extern "C" void opt_async_generator(benchmark::State& state) {
    [&]() -> conduit::coroutine {
        auto gen = nums<cppcoro::async_generator<long>>();
        auto it = std::begin(gen);
        long value = 0;
        for (auto _ : state) {
            value = *co_await it;
        }
        benchmark::DoNotOptimize(value);
    }();
}


extern "C" auto (*get_generator)()
    -> cppcoro::generator<long> = nums<cppcoro::generator<long>>;

extern "C" void noinline_generator(benchmark::State& state) {
    using std::begin;
    auto gen = get_generator();
    auto it = begin(gen);
    for (auto _ : state) {
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}

extern "C" void noinline_checked_generator(benchmark::State& state) {
    auto gen = get_generator();
    auto it = gen.begin();
    auto end = gen.end();
    for (auto _ : state) {
        if(it == end) break;
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}

extern "C" void checked_generator(benchmark::State& state) {
    auto gen = nums<cppcoro::generator<long>>();
    auto it = gen.begin();
    auto end_ = gen.end();
    for (auto _ : state) {
        if(it == end_) break;
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}


extern "C" auto (*get_async_generator)()
    -> cppcoro::async_generator<long> = nums<cppcoro::async_generator<long>>;

extern "C" void noinline_async_generator(benchmark::State& state) {
    [&]() -> conduit::coroutine {
        auto gen = get_async_generator();
        auto it = std::begin(gen);
        for (auto _ : state) {
            long value = *co_await it;
            benchmark::DoNotOptimize(value);
        }
    }();
}

#include <benchmark/benchmark.h>
#include <conduit/coroutine.hpp>
#include <conduit/generator.hpp>
#include <conduit/source.hpp>

template <class Gen>
auto nums() -> Gen {
    long i = 0;
    for (;; i++) {
        co_yield i;
    }
}

extern "C" void source(benchmark::State& state) {
    [&]() -> conduit::coroutine {
        auto gen = nums<conduit::source<long>>();
        for (auto _ : state) {
            long value = *co_await gen;
            benchmark::DoNotOptimize(value);
        }
    }();
}

extern "C" void opt_source(benchmark::State& state) {
    [&]() -> conduit::coroutine {
        auto gen = nums<conduit::source<long>>();
        long value = 0;
        for (auto _ : state) {
            value = *co_await gen;
        }
        benchmark::DoNotOptimize(value);
    }();
}

extern "C" void generator(benchmark::State& state) {
    using std::begin;
    auto gen = nums<conduit::generator<long>>();
    auto it = begin(gen);
    for (auto _ : state) {
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}

extern "C" void checked_generator(benchmark::State& state) {
    using std::begin;
    using std::end;
    auto gen = nums<conduit::generator<long>>();
    auto it = begin(gen);
    auto end_ = end(gen);
    for (auto _ : state) {
        if(it == end_) break;
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}


extern "C" void opt_generator(benchmark::State& state) {
    using std::begin;
    auto gen = nums<conduit::generator<long>>();
    auto it = begin(gen);
    long value = 0;
    for (auto _ : state) {
        value = *it;
        it++;
    }
    benchmark::DoNotOptimize(value);
}

extern "C" auto (*get_generator)()
    -> conduit::generator<long> = nums<conduit::generator<long>>;

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
    auto it = begin(gen);
    auto end_ = end(gen);
    for (auto _ : state) {
        if(it == end_) break;
        auto value = *it;
        it++;
        benchmark::DoNotOptimize(value);
    }
}


extern "C" auto (*get_source)()
    -> conduit::source<long> = nums<conduit::source<long>>;

extern "C" void noinline_source(benchmark::State& state) {
    [&]() -> conduit::coroutine {
        auto gen = get_source();
        for (auto _ : state) {
            long value = *co_await gen;
            benchmark::DoNotOptimize(value);
        }
    }();
}

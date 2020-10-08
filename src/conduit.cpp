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

extern "C" void source(benchmark::State& state) {
    [&]() -> conduit::coroutine {
        auto gen = nums<conduit::source<long>>();
        for (auto _ : state) {
            long value = *co_await gen;
            benchmark::DoNotOptimize(value);
        }
    }();
}

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

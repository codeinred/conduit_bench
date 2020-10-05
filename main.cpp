#include <fmt/core.h>

#include <tuplet/tuplet.hpp>

#include <benchmark/benchmark.h>

#include <algorithm>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Used for dlopen, dlclose, and dlsym
#include <dlfcn.h>

using bench_fn = void (*)(benchmark::State&);
constexpr auto close_dl_handle = [](void* pointer) {
    if (pointer)
        dlclose(pointer);
};
using dl_handle_deleter = std::decay_t<decltype(close_dl_handle)>;
class library : std::unique_ptr<void, dl_handle_deleter> {
    using super = std::unique_ptr<void, dl_handle_deleter>;

   public:
    using super::operator bool;

    library() = default;
    library(const char* libname, int flags)
      : super(dlopen(libname, flags)) {}
    library(std::string const& libname, int flags)
      : super(dlopen(libname.c_str(), flags)) {}
    library(library&&) = default;

    library& operator=(library&&) = default;
    auto get_func(const char* funcname) -> bench_fn {
        return bench_fn(dlsym(super::get(), funcname));
    }
    auto get_func(std::string s) -> bench_fn { return get_func(s.c_str()); }
};

void check(auto& list) {
    for (auto& thing : list) {
        if (!thing) {
            throw std::logic_error("Null element");
        }
    }
}

using tuplet::tuple;
auto make_elem(std::string name, auto const& funcs) {
    auto libfile = "lib/lib" + name + ".so";
    auto lib = library(libfile, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
    std::vector<tuple<std::string, bench_fn>> to_bench;

    for (auto& f : funcs) {
        auto func_ptr = lib.get_func(f);
        if (!func_ptr) {
            throw std::logic_error(
                fmt::format("lib{}: function '{}' not found", name, f));
        }
        to_bench.push_back(tuple {name + "." + f, func_ptr});
    }
    auto elem = tuple {std::move(lib), std::move(to_bench)};

    return std::pair {name, std::move(elem)};
}
using value_t = tuple<library, std::vector<tuple<std::string, bench_fn>>>;

void run_benchmarks(int repetitions) {
    using tuplet::tuple;

    auto funcs = std::unordered_map<std::string, value_t>();
    funcs.emplace(
        make_elem("baseline", std::vector {"baseline", "std_function"}));
    funcs.emplace(
        make_elem("cppcoro", std::vector {"generator", "async_generator"}));
    funcs.emplace(
        make_elem("conduit-main", std::vector {"generator", "source"}));
    funcs.emplace(
        make_elem("conduit-dev", std::vector {"generator", "source"}));

    std::vector<tuple<const char*, bench_fn>> benchmarks;
    for (auto&& [key, value] : funcs) {
        auto&& [lib, funcs] = value;
        for (auto& [name, func] : funcs) {
            for (int i = 0; i < repetitions; i++) {
                benchmarks.push_back(tuple {name.c_str(), func});
            }
        }
    }

    std::random_device hardware_rng;
    auto seed = std::seed_seq(
        {hardware_rng(),
         hardware_rng(),
         hardware_rng(),
         hardware_rng(),
         hardware_rng(),
         hardware_rng(),
         hardware_rng(),
         hardware_rng(),
         hardware_rng(),
         hardware_rng()});
    std::mt19937_64 rng(seed);
    std::shuffle(benchmarks.begin(), benchmarks.end(), rng);

    for(auto [name, func] : benchmarks) {
        benchmark::RegisterBenchmark(name, func);
    }
    benchmark::RunSpecifiedBenchmarks();
}

int main() {

    run_benchmarks(10);
}

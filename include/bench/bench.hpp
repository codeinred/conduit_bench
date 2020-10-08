#pragma once

#include <fmt/core.h>

#include <benchmark/benchmark.h>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <tuplet/tuplet.hpp>

// Used for dlopen, dlclose, and dlsym
#include <dlfcn.h>

// bench::bench_fn definition
// bench::dl_handle_deleter implementation
// bench::library implementation
namespace bench {
namespace fs = std::filesystem;
using bench_fn = void (*)(benchmark::State&);

struct dl_handle_deleter {
    void operator()(void* pointer) const noexcept {
        if (pointer)
            dlclose(pointer);
    }
};

class library : std::unique_ptr<void, dl_handle_deleter> {
    using super = std::unique_ptr<void, dl_handle_deleter>;

   public:
    constexpr static int default_flags = RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND;
    using super::operator bool;

    library() = default;
    library(const char* libname, int flags = default_flags)
      : super(dlopen(libname, flags)) {}
    library(std::string const& libname, int flags = default_flags)
      : super(dlopen(libname.c_str(), flags)) {}
    library(library&&) = default;

    void* sym(const char* str) { return dlsym(super::get(), str); }
    template <class T>
    T sym_as(const char* str) {
        return T(dlsym(super::get(), str));
    }

    library& operator=(library&&) = default;
};
} // namespace bench

namespace bench {
fs::path deref_path(fs::path p) {
    while (fs::is_symlink(p)) {
        p = fs::read_symlink(p);
    }
    return std::move(p);
}
fs::path get_program_dir() {
    static auto program_dir = deref_path("/proc/self/exe").parent_path();
    return program_dir;
}
fs::path get_lib_dir() {
    static auto libdir = (get_program_dir() / "../lib").lexically_normal();
    return libdir;
}

} // namespace bench

// bench::context implementation
namespace bench {
using tuplet::tuple;

class context {
    std::vector<library> sources;
    std::vector<tuple<std::string, bench_fn>> benchmarks;

   public:
    void add_library(fs::path lib, std::initializer_list<const char*> funcs) {
        auto lib_name = lib.stem().string();
        auto group_name = std::string_view(lib_name).substr(3);
        if (auto l = library(lib.c_str())) {
            for (auto func_name : funcs) {
                if (auto func = l.sym_as<bench_fn>(func_name)) {
                    benchmarks.push_back(tuple {
                        fmt::format("{}.{}", group_name, func_name), func});
                } else {
                    throw std::logic_error(fmt::format(
                        "Could not find {} in {}", func_name, lib.c_str()));
                }
            }
            sources.push_back(std::move(l));
        } else {
            throw std::logic_error(
                fmt::format("Could not open '{}' via dlopen", lib.c_str()));
        }
    }
    void add_group(const char* name, std::initializer_list<const char*> funcs) {
        add_library(
            fmt::format("{}/lib{}.so", get_lib_dir().string(), name), funcs);
    }

    void register_all(bool shuffle = true, int repetitions = 1) {
        std::vector<tuple<const char*, bench_fn>> bench_list(
            repetitions * benchmarks.size());

        {
            auto scan = bench_list.begin();
            for (auto& [name, func] : benchmarks) {
                for (int i = 0; i < repetitions; i++) {
                    *scan++ = {name.c_str(), func};
                }
            }
        }

        if (shuffle) {
            std::random_device hardware_rng;
            auto seed = std::seed_seq {
                hardware_rng(),
                hardware_rng(),
                hardware_rng(),
                hardware_rng(),
                hardware_rng(),
                hardware_rng(),
                hardware_rng(),
                hardware_rng(),
                hardware_rng(),
                hardware_rng()};

            std::shuffle(
                bench_list.begin(), bench_list.end(), std::mt19937_64(seed));
        }

        for (auto& [name, func] : bench_list) {
            benchmark::RegisterBenchmark(name, func);
        }
    }
};
} // namespace bench

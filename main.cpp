#include <arglet/arglet.hpp>
#include <bench/bench.hpp>

namespace tags {
using arglet::tag;
constexpr tag<0> repetitions;
constexpr tag<1> shuffle;
} // namespace tags
int main(int argc, char** argv) {
    using namespace arglet;
    auto parser = sequence {
        ignore_arg,
        group {
            prefixed_value {tags::repetitions, 'r', "--repetitions=", 1},
            flag {tags::shuffle, 's', "--shuffle"},
            string {tags::tag<2>()}}};
    parser.parse(argc, (char const**)argv);

    bench::context context;

    context.add_group(
        "baseline",
        {"baseline",
         "std_function",
         "function_pointer",
         "opt_std_function",
         "opt_function_pointer"});

    context.add_group(
        "conduit-main",
        {"generator",
         "source",
         "checked_generator",
         "opt_generator",
         "opt_source",
         "noinline_generator",
         "noinline_source"});

    context.add_group(
        "conduit-dev",
        {"generator",
         "source",
         "checked_generator",
         "opt_generator",
         "opt_source",
         "noinline_generator",
         "noinline_source"});

    context.add_group(
        "cppcoro",
        {"generator",
         "async_generator",
         "checked_generator",
         "opt_generator",
         "opt_async_generator",
         "noinline_generator",
         "noinline_async_generator"});

    context.register_all(parser[tags::shuffle], parser[tags::repetitions]);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}

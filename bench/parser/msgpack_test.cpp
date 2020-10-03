#include <msgpack.hpp>

#include "common.h"
#include "mprpc/generate_string_data.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/transport/msgpack/msgpack_parser.h"

static void parse_msgpack(benchmark::State& state) {
    static auto logger = mprpc::logging::create_file_logger(
        "mprpc_bench_parser", "mprpc_bench_parser.log");
    auto parser =
        std::make_shared<mprpc::transport::msgpack::msgpack_parser>(logger);

    auto size = static_cast<std::size_t>(state.range());
    auto data = mprpc::generate_string_data(size);

    for (auto _ : state) {
        parser->parse(data.data(), data.size());
    }
}
BENCH_PARSER(parse_msgpack);

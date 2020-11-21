#include <msgpack.hpp>

#include "common.h"
#include "mprpc/generate_string_data.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/transport/parsers/msgpack_parser.h"

static void parse_msgpack(benchmark::State& state) {
    static auto logger = mprpc::logging::create_stdout_logger();
    auto parser = mprpc::transport::parsers::create_msgpack_parser(logger);

    auto size = static_cast<std::size_t>(state.range());
    auto data = mprpc::generate_string_data(size);

    for (auto _ : state) {
        parser->parse(data.data(), data.size());
    }
}
BENCH_PARSER(parse_msgpack);

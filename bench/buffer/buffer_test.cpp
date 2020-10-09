#include "mprpc/buffer.h"

#include <vector>

#include <benchmark/benchmark.h>

// NOLINTNEXTLINE: use of external library
#define BENCH_BUFFER(FUNCTION) \
    BENCHMARK(FUNCTION)->RangeMultiplier(32)->Range(1, 1024)

constexpr std::size_t repetition = 1000;

static void resize_std_vector_char(benchmark::State& state) {
    const auto buf_unit = static_cast<std::size_t>(state.range());
    for (auto _ : state) {
        std::vector<char> buf;
        for (std::size_t i = 0; i < repetition; ++i) {
            buf.resize(buf.size() + buf_unit);
        }
    }
}
BENCH_BUFFER(resize_std_vector_char);

static void resize_mprpc_buffer(benchmark::State& state) {
    const auto buf_unit = static_cast<std::size_t>(state.range());
    for (auto _ : state) {
        mprpc::buffer buf;
        for (std::size_t i = 0; i < repetition; ++i) {
            buf.resize(buf.size() + buf_unit);
        }
    }
}
BENCH_BUFFER(resize_mprpc_buffer);

static void consume_std_vector_char(benchmark::State& state) {
    const auto buf_unit = static_cast<std::size_t>(state.range());
    const std::size_t initial_size = buf_unit * repetition;
    for (auto _ : state) {
        auto buf = std::vector<char>(initial_size);
        for (std::size_t i = 0; i < repetition; ++i) {
            buf.erase(buf.begin(), buf.begin() + buf_unit);
        }
    }
}
BENCH_BUFFER(consume_std_vector_char);

static void consume_mprpc_buffer(benchmark::State& state) {
    const auto buf_unit = static_cast<std::size_t>(state.range());
    const std::size_t initial_size = buf_unit * repetition;
    for (auto _ : state) {
        mprpc::buffer buf{initial_size};
        for (std::size_t i = 0; i < repetition; ++i) {
            buf.consume(buf_unit);
        }
    }
}
BENCH_BUFFER(consume_mprpc_buffer);

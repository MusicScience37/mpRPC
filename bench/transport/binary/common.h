#pragma once

#include <benchmark/benchmark.h>

#define BENCH_TRANSPORT(FUNCTION) \
    BENCHMARK(FUNCTION)->RangeMultiplier(32)->Range(1, 1024 * 1024)

inline void set_counters(benchmark::State& state) {
    state.counters["MessageRate"] =
        benchmark::Counter(1, benchmark::Counter::kIsIterationInvariantRate);
    state.counters["ByteRate"] = benchmark::Counter(
        state.range() * 2.0, benchmark::Counter::kIsIterationInvariantRate);
}

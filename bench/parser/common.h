#pragma once

#include <benchmark/benchmark.h>

#define BENCH_PARSER(FUNCTION) BENCHMARK(FUNCTION)->Arg(128)

/*
 * Copyright 2020 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief test of shared_any class
 */
#include "stl_ext/shared_any.h"

#include <vector>

#include <benchmark/benchmark.h>

std::vector<char> make_vector(std::size_t size) {
    std::vector<char> data;
    for (std::size_t i = 0; i < size; ++i) {
        data.push_back(static_cast<char>(i));
    }
    return data;
}

constexpr int base = 32;
constexpr int max_size = 1024 * 1024;

// NOLINTNEXTLINE: use of external library
#define BENCH_VECTOR(FUNCTION) \
    BENCHMARK(FUNCTION)->RangeMultiplier(base)->Range(1, max_size)

static void copy_vector(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());
    const auto data = make_vector(size);

    for (auto _ : state) {
        std::vector<char> copy = data;
        benchmark::DoNotOptimize(copy);
    }
}
BENCH_VECTOR(copy_vector);

static void copy_shared_any_vector(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());
    const stl_ext::shared_any data = make_vector(size);

    for (auto _ : state) {
        stl_ext::shared_any copy = data;
        benchmark::DoNotOptimize(copy);
    }
}
BENCH_VECTOR(copy_shared_any_vector);

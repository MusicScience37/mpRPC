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
 * \brief test of pod_vector class
 */
#include "stl_ext/pod_vector.h"

#include <vector>

#include <benchmark/benchmark.h>

constexpr int base = 32;
constexpr int max_size = 1024;
constexpr std::size_t size_unit = 1024;

// NOLINTNEXTLINE: use of external library
#define BENCH_VECTOR(FUNCTION) \
    BENCHMARK(FUNCTION)->RangeMultiplier(base)->Range(1, max_size)

static void push_back_std_vector_char(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());

    for (auto _ : state) {
        std::vector<char> vec;
        for (std::size_t i = 0; i < size; ++i) {
            vec.push_back('a');
        }
        if (vec.size() != size) {
            state.SkipWithError("vector size error");
        }
    }
}
BENCH_VECTOR(push_back_std_vector_char);

static void push_back_pod_vector_char(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());

    for (auto _ : state) {
        stl_ext::pod_vector<char> vec;
        for (std::size_t i = 0; i < size; ++i) {
            vec.push_back('a');
        }
        if (vec.size() != size) {
            state.SkipWithError("vector size error");
        }
    }
}
BENCH_VECTOR(push_back_pod_vector_char);

static void insert_back_std_vector_char(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());

    const auto data = std::vector<char>(size_unit, 'a');
    for (auto _ : state) {
        std::vector<char> vec;
        for (std::size_t i = 0; i < size; ++i) {
            vec.insert(vec.end(), data.begin(), data.end());
        }
        if (vec.size() != size * size_unit) {
            state.SkipWithError("vector size error");
        }
    }
}
BENCH_VECTOR(insert_back_std_vector_char);

static void insert_back_pod_vector_char(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());

    const auto data = std::vector<char>(size_unit, 'a');
    for (auto _ : state) {
        stl_ext::pod_vector<char> vec;
        for (std::size_t i = 0; i < size; ++i) {
            vec.insert(vec.end(), data.begin(), data.end());
        }
        if (vec.size() != size * size_unit) {
            state.SkipWithError("vector size error");
        }
    }
}
BENCH_VECTOR(insert_back_pod_vector_char);

static void consume_std_vector_char(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());

    const auto data = std::vector<char>(size * size_unit, 'a');
    for (auto _ : state) {
        std::vector<char> vec(data);
        for (std::size_t i = 0; i < size; ++i) {
            vec.erase(vec.begin(), vec.begin() + size_unit);
        }
        if (!vec.empty()) {
            state.SkipWithError("vector size error");
        }
    }
}
BENCH_VECTOR(consume_std_vector_char);

static void consume_pod_vector_char(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());

    const auto data_orig = std::vector<char>(size * size_unit, 'a');
    const auto data =
        stl_ext::pod_vector<char>(data_orig.begin(), data_orig.size());
    for (auto _ : state) {
        stl_ext::pod_vector<char> vec(data);
        for (std::size_t i = 0; i < size; ++i) {
            vec.erase(vec.begin(), vec.begin() + size_unit);
        }
        if (!vec.empty()) {
            state.SkipWithError("vector size error");
        }
    }
}
BENCH_VECTOR(consume_pod_vector_char);

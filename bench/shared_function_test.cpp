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
 * \brief test of shared_function class
 */
#include "stl_ext/shared_function.h"

#include <functional>
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

static void copy_function_vector(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());
    using function_type = std::function<std::vector<char>()>;
    const function_type func = [data = make_vector(size)] { return data; };

    for (auto _ : state) {
        function_type copy = func;
        benchmark::DoNotOptimize(copy);
    }
}
BENCH_VECTOR(copy_function_vector);

static void copy_shared_function_vector(benchmark::State& state) {
    const auto size = static_cast<std::size_t>(state.range());
    using function_type = stl_ext::shared_function<std::vector<char>()>;
    const function_type func = [data = make_vector(size)] { return data; };

    for (auto _ : state) {
        function_type copy = func;
        benchmark::DoNotOptimize(copy);
    }
}
BENCH_VECTOR(copy_shared_function_vector);

static void invoke_lambda(benchmark::State& state) {
    const auto func = [](int x, int y) { return x + y; };

    const volatile int x = 3;
    const volatile int y = 5;
    for (auto _ : state) {
        volatile int res = 0;
        res = func(x, y);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(invoke_lambda);

static void invoke_function(benchmark::State& state) {
    using function_type = std::function<int(int, int)>;
    const function_type func = [](int x, int y) { return x + y; };

    const volatile int x = 3;
    const volatile int y = 5;
    for (auto _ : state) {
        volatile int res = 0;
        res = func(x, y);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(invoke_function);

static void invoke_shared_function(benchmark::State& state) {
    using function_type = stl_ext::shared_function<int(int, int)>;
    const function_type func = [](int x, int y) { return x + y; };

    const volatile int x = 3;
    const volatile int y = 5;
    for (auto _ : state) {
        volatile int res = 0;
        res = func(x, y);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(invoke_shared_function);

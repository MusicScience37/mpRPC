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
 * \brief test of RPC on UDP
 */
#include "common.h"
#include "mprpc/client_builder.h"
#include "mprpc/generate_string.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/method_client.h"
#include "mprpc/server_builder.h"

static void echo_udp(benchmark::State& state) {
    static const auto logger =
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn);

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    const auto size = static_cast<std::size_t>(state.range());
    const auto data = mprpc::generate_string(size);

    try {
        auto server = mprpc::server_builder(logger)
                          .num_threads(1)
                          .listen_udp(host, port)
                          .method<std::string(std::string)>(
                              "echo", [](std::string str) { return str; })
                          .create();

        const auto duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(duration);

        auto client = mprpc::client_builder(logger)
                          .num_threads(1)
                          .connect_udp(host, port)
                          .create();

        auto echo_client =
            mprpc::method_client<std::string(std::string)>(*client, "echo");

        for (auto _ : state) {
            const auto res = echo_client(data);
            benchmark::DoNotOptimize(res);
        }
    } catch (const std::exception& err) {
        MPRPC_ERROR(logger, err.what());
        state.SkipWithError(err.what());
    }

    set_counters(state);
}
// NOLINTNEXTLINE
BENCHMARK(echo_udp)->UseRealTime()->RangeMultiplier(32)->Range(1, 1024 * 32);

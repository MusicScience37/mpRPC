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
 * \brief test of method execution
 */
#include <future>

#include "common.h"
#include "mprpc/execution/function_method_executor.h"
#include "mprpc/execution/simple_method_server.h"
#include "mprpc/generate_string.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/transport/mock/mock_session.h"

static void echo_exec(benchmark::State& state) {
    static const auto logger = mprpc::logging::labeled_logger(
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn));

    const auto size = static_cast<std::size_t>(state.range());
    const auto data = mprpc::generate_string(size);

    try {
        const auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
        threads->start();

        std::vector<std::shared_ptr<mprpc::execution::method_executor>>
            methods = {
                mprpc::execution::make_function_method_executor<std::string(
                    std::string)>(
                    logger, "echo", [](std::string str) { return str; })};

        const auto server =
            std::make_shared<mprpc::execution::simple_method_server>(
                logger, *threads, methods);

        const auto session =
            std::make_shared<mprpc::transport::mock::mock_session>(
                threads->context());

        for (auto _ : state) {
            const auto request = mprpc::message(
                mprpc::pack_request(0, "echo", std::forward_as_tuple(data)));

            std::promise<mprpc::message_data> promise;
            auto future = promise.get_future();
            server->async_process_message(session, request,
                [&promise](const mprpc::error_info& error, bool response_exists,
                    const mprpc::message_data& message) {
                    if (error) {
                        promise.set_exception(
                            std::make_exception_ptr(mprpc::exception(error)));
                    } else if (response_exists) {
                        promise.set_value(message);
                    } else {
                        promise.set_value(mprpc::message_data());
                    }
                });

            const auto response = mprpc::message(future.get());
            const auto result = response.result_as<std::string>();
            benchmark::DoNotOptimize(result);
        }
    } catch (const std::exception& err) {
        MPRPC_ERROR(logger, err.what());
        state.SkipWithError(err.what());
    }

    set_counters(state);
}
BENCH_ECHO(echo_exec);

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
 * \brief implementation of thread_pool class
 */
#include "mprpc/thread_pool.h"

#include <thread>
#include <utility>
#include <vector>

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/post.hpp>
#include <stl_ext/shared_function.h>

#include "mprpc/error_info.h"
#include "mprpc/exception.h"
#include "mprpc/logging/logging_macros.h"

namespace mprpc {

struct thread_pool::impl {
    //! io_context object
    asio::io_context context;

    //! object to control executor work
    asio::executor_work_guard<asio::io_context::executor_type> work_guard;

    //! number of threads
    std::size_t num_threads;

    //! threads
    std::vector<std::thread> threads;

    //! handler called when errors happen
    on_error_handler_type on_error;

    //! logger
    std::shared_ptr<logging::logger> logger;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param num_threads number of threads
     */
    impl(std::shared_ptr<logging::logger> logger_in, std::size_t num_threads_in)
        : context(static_cast<int>(num_threads_in)),
          work_guard(asio::make_work_guard(context)),
          num_threads(num_threads_in),
          logger(std::move(logger_in)) {}
};

thread_pool::thread_pool(
    std::shared_ptr<logging::logger> logger, std::size_t num_threads)
    : impl_(std::make_unique<impl>(std::move(logger), num_threads)) {}

void thread_pool::on_error(on_error_handler_type handler) {
    impl_->on_error = std::move(handler);
}

void thread_pool::start() {
    if (impl_->threads.empty()) {
        impl_->threads.reserve(impl_->num_threads);
        for (std::size_t i = 0; i < impl_->num_threads; ++i) {
            impl_->threads.emplace_back(&thread_pool::process_in_thread, this);
        }
        MPRPC_DEBUG(impl_->logger, "started {} threads", impl_->num_threads);
    }
}

void thread_pool::stop() {
    if (impl_->context.stopped()) {
        return;
    }
    impl_->work_guard.reset();
    impl_->context.stop();
    for (auto& thread : impl_->threads) {
        thread.join();
    }
    impl_->threads.clear();
    MPRPC_DEBUG(impl_->logger, "stopped {} threads", impl_->num_threads);
}

asio::io_context& thread_pool::context() { return impl_->context; }

void thread_pool::post(function_type function) {
    asio::post(impl_->context, std::move(function));
}

thread_pool::~thread_pool() { stop(); }

void thread_pool::process_in_thread() {
    try {
        impl_->context.run();
        MPRPC_TRACE(impl_->logger, "finishing thread normally");
    } catch (const exception& e) {
        process_thread_error(e.info());
    } catch (const std::exception& e) {
        process_thread_error(
            error_info(error_code::unexpected_error, e.what()));
    }
}

void thread_pool::process_thread_error(const error_info& error) {
    MPRPC_FATAL(impl_->logger, "error in thread: {}", error);
    if (impl_->on_error) {
        impl_->on_error(error);
    }
}

}  // namespace mprpc

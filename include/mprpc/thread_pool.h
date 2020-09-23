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
 * \brief declaration and implementation of thread_pool class
 */
#pragma once

#include <functional>
#include <thread>
#include <vector>

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>

#include "mprpc/error_info.h"
#include "mprpc/exception.h"
#include "mprpc/logging/logger.h"
#include "mprpc/logging/logging_macros.h"

namespace mprpc {

/*!
 * \brief class of threads to run asio::io_context::run
 */
class thread_pool {
public:
    //! type of handlers called when errors happen
    using on_error_handler_type = std::function<void(const error_info&)>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param num_threads number of threads
     */
    explicit thread_pool(
        std::shared_ptr<logging::logger> logger, std::size_t num_threads = 1)
        : context_(static_cast<int>(num_threads)),
          work_guard_(asio::make_work_guard(context_)),
          num_threads_(num_threads),
          logger_(std::move(logger)) {}

    /*!
     * \brief set handler called when errors happen
     *
     * \param handler handler
     *
     * \warning Call this before starting threads.
     */
    void on_error(on_error_handler_type handler) {
        on_error_ = std::move(handler);
    }

    /*!
     * \brief start threads if not started
     *
     * \note This function is not thread safe for the same object.
     */
    void start() {
        if (threads_.empty()) {
            threads_.reserve(num_threads_);
            for (std::size_t i = 0; i < num_threads_; ++i) {
                threads_.emplace_back(&thread_pool::process_in_thread, this);
            }
            MPRPC_DEBUG(logger_, "started {} threads", num_threads_);
        }
    }

    /*!
     * \brief stop threads if not stopped
     *
     * \note This function is not thread safe for the same object.
     */
    void stop() {
        if (context_.stopped()) {
            return;
        }
        work_guard_.reset();
        context_.stop();
        for (auto& thread : threads_) {
            thread.join();
        }
        threads_.clear();
        MPRPC_DEBUG(logger_, "stopped {} threads", num_threads_);
    }

    /*!
     * \brief get io_context object
     *
     * \return io_context object
     */
    asio::io_context& context() { return context_; }

    /*!
     * \brief destruct
     *
     */
    ~thread_pool() { stop(); }

    thread_pool(const thread_pool&) = delete;
    thread_pool(thread_pool&&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;
    thread_pool& operator=(thread_pool&&) = delete;

private:
    /*!
     * \brief process in thread
     */
    void process_in_thread() {
        try {
            context_.run();
            MPRPC_TRACE(logger_, "finishing thread normally");
        } catch (const exception& e) {
            process_thread_error(e.info());
        } catch (const std::exception& e) {
            process_thread_error(
                error_info(error_code::unexpected_error, e.what()));
        }
    }

    /*!
     * \brief process errors
     *
     * \param error error
     */
    void process_thread_error(const error_info& error) {
        MPRPC_FATAL(logger_, "error in thread: {}", error);
        if (on_error_) {
            on_error_(error);
        }
    }

    //! io_context object
    asio::io_context context_;

    //! object to control executor work
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_;

    //! number of threads
    std::size_t num_threads_;

    //! threads
    std::vector<std::thread> threads_{};

    //! handler called when errors happen
    on_error_handler_type on_error_{};

    //! logger
    std::shared_ptr<logging::logger> logger_;
};

}  // namespace mprpc

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
 * \brief declaration of thread_pool class
 */
#pragma once

#include <memory>

#include <asio/io_context.hpp>
#include <stl_ext/shared_function.h>

#include "mprpc/error_info.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/logging/logger.h"
#include "mprpc/mprpc_export.h"

namespace mprpc {

/*!
 * \brief class of threads to run asio::io_context::run
 */
class MPRPC_EXPORT thread_pool {
public:
    //! type of handlers called when errors happen
    using on_error_handler_type =
        stl_ext::shared_function<void(const error_info&)>;

    //! type of functions to execute
    using function_type = stl_ext::shared_function<void()>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param num_threads number of threads
     */
    explicit thread_pool(
        std::shared_ptr<logging::logger> logger, std::size_t num_threads = 1);

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param num_threads number of threads
     */
    explicit thread_pool(
        logging::labeled_logger logger, std::size_t num_threads = 1);

    /*!
     * \brief set handler called when errors happen
     *
     * \param handler handler
     *
     * \warning Call this before starting threads.
     */
    void on_error(on_error_handler_type handler);

    /*!
     * \brief start threads if not started
     *
     * \note This function is not thread safe for the same object.
     */
    void start();

    /*!
     * \brief stop threads if not stopped
     *
     * \note This function is not thread safe for the same object.
     */
    void stop();

    /*!
     * \brief get io_context object
     *
     * \return io_context object
     */
    asio::io_context& context();

    /*!
     * \brief add a function to be executed
     *
     * \param function function
     */
    void post(function_type function);

    /*!
     * \brief destruct
     *
     */
    ~thread_pool();

    thread_pool(const thread_pool&) = delete;
    thread_pool(thread_pool&&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;
    thread_pool& operator=(thread_pool&&) = delete;

private:
    /*!
     * \brief process in thread
     */
    void process_in_thread();

    /*!
     * \brief process errors
     *
     * \param error error
     */
    void process_thread_error(const error_info& error);

    //! struct for internal data
    struct impl;

    //! internal data
    impl* impl_{};
};

}  // namespace mprpc

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
 * \brief declaration and implementation of python_client_helper class
 */
#pragma once

#include <functional>
#include <memory>

#include <pybind11/pybind11.h>

#include "mprpc/client_config.h"
#include "mprpc/exception.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/require_nonull.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport.h"

namespace mprpc {
namespace python {

/*!
 * \brief class for implementation of clients in Python
 */
class python_client_helper {
public:
    /*!
     * \brief type of function to process messages
     *
     * Parameters:
     *
     * - error
     * - message data
     */
    using message_processor_t =
        std::function<void(const error_info&, const message_data&)>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param threads threads
     * \param connector connector
     * \param config configuration
     * \param message_processor function to process messages
     */
    python_client_helper(logging::labeled_logger logger,
        std::shared_ptr<thread_pool> threads,
        std::shared_ptr<transport::connector> connector, client_config config,
        message_processor_t message_processor)
        : labeled_logger_(std::move(logger)),
          threads_(std::move(threads)),
          connector_(std::move(connector)),
          config_(std::move(config)),
          message_processor_(std::move(message_processor)) {}

    /*!
     * \brief start process
     */
    void start() {
        pybind11::gil_scoped_release gil_released;
        MPRPC_INFO(labeled_logger_, "client starting");
        threads_->start();
        do_read();
        MPRPC_INFO(labeled_logger_, "client started");
    }

    /*!
     * \brief stop process
     */
    void stop() {
        pybind11::gil_scoped_release gil_released;
        MPRPC_INFO(labeled_logger_, "client stopping");
        connector_->shutdown();
        threads_->stop();
        MPRPC_INFO(labeled_logger_, "client stopped");
    }

    /*!
     * \brief asynchronously send a request
     *
     * \param data message data
     * \param handler handler on message sended
     */
    void async_send(const message_data& data,
        const stl_ext::shared_function<void(const error_info&)>& handler) {
        connector_->async_write(data, handler);
    }

    /*!
     * \brief create a client object
     *
     * \param logger logger
     * \param config configuration
     * \param message_processor function to process messages
     * \return client object
     */
    static std::shared_ptr<python_client_helper> create(
        const std::shared_ptr<logging::logger>& logger,
        const client_config& config,
        const message_processor_t& message_processor) {
        pybind11::gil_scoped_release gil_released;
        const auto labeled_logger =
            logging::labeled_logger(logger, "mprpc.client");

        const auto threads = std::make_shared<thread_pool>(
            labeled_logger, config.num_threads.value());

        std::shared_ptr<transport::connector> connector;
        switch (config.connector_type.value()) {
        case transport_type::tcp:
            connector =
                transport::tcp::create_tcp_connector(labeled_logger, *threads,
                    transport::create_compressor_factory(
                        config.tcp_connector.compression),
                    transport::create_parser_factory(
                        config.tcp_connector.compression),
                    config.tcp_connector);
            break;
        case transport_type::udp:
            connector =
                transport::udp::create_udp_connector(labeled_logger, *threads,
                    transport::create_compressor_factory(
                        config.udp_connector.compression),
                    transport::create_parser_factory(
                        config.udp_connector.compression),
                    config.udp_connector);
            break;
        }

        return std::make_shared<python_client_helper>(labeled_logger, threads,
            std::move(connector), config, message_processor);
    }

    python_client_helper(const python_client_helper&) = delete;
    python_client_helper& operator=(const python_client_helper&) = delete;
    python_client_helper(python_client_helper&&) = delete;
    python_client_helper& operator=(python_client_helper&&) = delete;

    //! destruct
    ~python_client_helper() { stop(); }

private:
    /*!
     * \brief read a message
     */
    void do_read() {
        connector_->async_read(
            [this](const error_info& error, const message_data& data) {
                message_processor_(error, data);
            });
    }

    //! logger
    logging::labeled_logger labeled_logger_;

    //! thread pool
    std::shared_ptr<thread_pool> threads_;

    //! connector
    std::shared_ptr<transport::connector> connector_;

    //! configuration
    const client_config config_;

    //! function to process messages
    message_processor_t message_processor_;
};

}  // namespace python
}  // namespace mprpc

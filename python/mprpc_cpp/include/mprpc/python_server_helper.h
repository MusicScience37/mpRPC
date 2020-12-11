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
 * \brief declaration and implementation of python_server_helper class
 */
#pragma once

#include <functional>

#include "mprpc/exception.h"
#include "mprpc/impl/server_base.h"

namespace mprpc {
namespace python {

/*!
 * \brief class for implementation of servers in Python
 */
class python_server_helper final : public impl::server_base {
public:
    /*!
     * \brief type of function to process messages
     *
     * Parameters:
     *
     * - session
     * - message data
     *
     * Return value: message data for response (empty for no response)
     */
    using message_processor_t = std::function<message_data(
        const std::shared_ptr<transport::session>&, const message_data&)>;

    /*!
     * \brief construct
     *
     * \param logger logger
     * \param threads thread pool
     * \param acceptors acceptors
     * \param config configuration
     * \param message_processor function to process messages
     */
    python_server_helper(const logging::labeled_logger& logger,
        const std::shared_ptr<mprpc::thread_pool>& threads,
        std::vector<std::shared_ptr<transport::acceptor>> acceptors,
        server_config config, const message_processor_t& message_processor)
        : impl::server_base(
              logger, threads, std::move(acceptors), std::move(config)),
          logger_(logger),
          threads_(threads) {}

    /*!
     * \brief asynchronously process message
     *
     * \param session session
     * \param data message data
     * \param handler handler on message processed
     */
    void async_process_message(
        const std::shared_ptr<transport::session>& session,
        const message_data& data,
        const execution::method_server::on_message_processed_handler& handler)
        override {
        threads_->post([this, session, data, handler]() {
            try {
                const auto response = message_processor_(session, data);
                handler(error_info(), response.has_data(), response);
            } catch (const exception& e) {
                handler(e.info(), false, message_data());
            } catch (const std::exception& e) {
                handler(error_info(error_code::unexpected_error, e.what()),
                    false, message_data());
            }
        });
    }

    /*!
     * \brief create an server helper object
     *
     * \param logger logger
     * \param config configuration
     * \param message_processor function to process messages
     * \return server helper object
     */
    static std::shared_ptr<python_server_helper> create(
        const std::shared_ptr<mprpc::logging::logger>& logger,
        const server_config& config,
        const message_processor_t& message_processor) {
        const auto labeled_logger =
            logging::labeled_logger(logger, "mprpc.server");
        const auto threads = std::make_shared<thread_pool>(
            labeled_logger, config.num_threads.value());

        const std::size_t num_acceptors =
            config.tcp_acceptors.size() + config.udp_acceptors.size();
        if (num_acceptors == 0) {
            throw exception(
                error_info(error_code::invalid_config_value, "no acceptors"));
        }

        std::vector<std::shared_ptr<transport::acceptor>> acceptors;
        acceptors.reserve(num_acceptors);
        for (const auto& config : config.tcp_acceptors) {
            acceptors.push_back(transport::tcp::create_tcp_acceptor(
                labeled_logger, *threads,
                transport::create_compressor_factory(config.compression),
                transport::create_parser_factory(config.compression), config));
        }
        for (const auto& config : config.udp_acceptors) {
            acceptors.push_back(transport::udp::create_udp_acceptor(
                labeled_logger, *threads,
                transport::create_compressor_factory(config.compression),
                transport::create_parser_factory(config.compression), config));
        }

        return std::make_shared<python_server_helper>(labeled_logger, threads,
            std::move(acceptors), config, message_processor);
    }

    python_server_helper(const python_server_helper&) = delete;
    python_server_helper& operator=(const python_server_helper&) = delete;
    python_server_helper(python_server_helper&&) = delete;
    python_server_helper& operator=(python_server_helper&&) = delete;

    //! destruct
    ~python_server_helper() { stop(); }

private:
    //! logger
    logging::labeled_logger logger_;

    //! thread pool
    std::shared_ptr<mprpc::thread_pool> threads_;

    //! function to process messages
    message_processor_t message_processor_;
};

}  // namespace python
}  // namespace mprpc

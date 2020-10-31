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
 * \brief declaration and implementation of server_builder class
 */
#pragma once

#include <functional>
#include <memory>

#include "mprpc/execution/function_method_executor.h"
#include "mprpc/execution/simple_method_server.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/server.h"
#include "mprpc/transport.h"

namespace mprpc {

/*!
 * \brief class to create server
 */
class server_builder {
public:
    //! construct
    server_builder() = default;

    /*!
     * \brief construct
     *
     * \param logger logger
     */
    explicit server_builder(std::shared_ptr<mprpc::logging::logger> logger)
        : logger_(std::move(logger)) {}

    /*!
     * \brief set number of threads
     *
     * \param num_threads number of threads
     * \return this object
     */
    server_builder& num_threads(std::size_t num_threads) {
        num_threads_ = num_threads;
        return *this;
    }

    /*!
     * \brief set to listen a TCP port
     *
     * \param ip_address IP address string (IPv4 or IPv6)
     * \param port port number
     * \return this object
     */
    server_builder& listen_tcp(
        const std::string& ip_address, std::uint16_t port) {
        acceptor_factories_.emplace_back(
            [ip_address, port](
                const std::shared_ptr<mprpc::logging::logger>& logger,
                thread_pool& threads,
                const std::shared_ptr<transport::parser_factory>&
                    parser_factory) {
                return transport::tcp::create_tcp_acceptor(
                    logger, ip_address, port, threads, parser_factory);
            });
        return *this;
    }

    /*!
     * \brief add a method
     *
     * \param method_executor method executor
     * \return this object
     */
    server_builder& method(
        std::shared_ptr<execution::method_executor> method_executor) {
        methods_.push_back(std::move(method_executor));
        return *this;
    }

    /*!
     * \brief add a method
     *
     * \tparam FunctionSignature function signature
     * \tparam Function function type
     * \param name method name
     * \param function function to execute method
     * \return this object
     */
    template <typename FunctionSignature, typename Function>
    server_builder& method(const std::string& name, Function&& function) {
        return method(
            execution::make_function_method_executor<FunctionSignature>(
                logger_, name, std::forward<Function>(function)));
    }

    /*!
     * \brief create server
     *
     * \return server
     */
    std::unique_ptr<server> create() {
        const auto threads =
            std::make_shared<thread_pool>(logger_, num_threads_);

        auto method_server = std::make_shared<execution::simple_method_server>(
            logger_, *threads, methods_);

        std::vector<std::shared_ptr<transport::acceptor>> acceptors;
        acceptors.reserve(acceptor_factories_.size());
        for (const auto& factory : acceptor_factories_) {
            acceptors.push_back(factory(logger_, *threads, parser_factory_));
        }

        auto srv = std::make_unique<server>(
            logger_, threads, std::move(acceptors), std::move(method_server));
        srv->start();

        return srv;
    }

private:
    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_{
        logging::create_stdout_logger(mprpc::logging::log_level::info)};

    //! number of threads
    std::size_t num_threads_{1};

    //! parser factory
    std::shared_ptr<transport::parser_factory> parser_factory_{
        std::make_shared<transport::parsers::msgpack_parser_factory>()};

    //! type of acceptor factory method
    using acceptor_factory_type =
        std::function<std::shared_ptr<transport::acceptor>(
            const std::shared_ptr<mprpc::logging::logger>&, thread_pool&,
            const std::shared_ptr<transport::parser_factory>&)>;

    //! acceptor factories
    std::vector<acceptor_factory_type> acceptor_factories_{};

    //! methods
    std::vector<std::shared_ptr<execution::method_executor>> methods_{};
};

}  // namespace mprpc

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

#include "mprpc/error_code.h"
#include "mprpc/exception.h"
#include "mprpc/execution/function_method_executor.h"
#include "mprpc/execution/simple_method_server.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/server.h"
#include "mprpc/transport.h"
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/create_compressor_factory.h"
#include "mprpc/transport/create_parser_factory.h"
#include "mprpc/transport/tcp/tcp.h"
#include "mprpc/transport/udp/udp.h"

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
        server_config_.num_threads = num_threads;
        return *this;
    }

    /*!
     * \brief set client configuration
     *
     * \param config configuration
     * \return this object
     */
    server_builder& server_config(mprpc::server_config config) {
        server_config_ = std::move(config);
        return *this;
    }

    /*!
     * \brief set to listen a TCP port
     *
     * \param config configuration
     * \return this object
     */
    server_builder& listen_tcp(
        const transport::tcp::tcp_acceptor_config& config =
            transport::tcp::tcp_acceptor_config()) {
        server_config_.tcp_acceptors.push_back(config);
        return *this;
    }

    /*!
     * \brief set to listen a TCP port
     *
     * \param ip_address IP address string (IPv4 or IPv6)
     * \param port port number
     * \param compression_type compression type
     * \return this object
     */
    server_builder& listen_tcp(const std::string& ip_address,
        std::uint16_t port,
        transport::compression_type compression_type =
            transport::compression_type::none) {
        transport::tcp::tcp_acceptor_config config;
        config.host = ip_address;
        config.port = port;
        config.compression.type = compression_type;
        listen_tcp(config);
        return *this;
    }

    /*!
     * \brief set to listen a UDP port
     *
     * \param config configuration
     * \return this object
     */
    server_builder& listen_udp(
        const transport::udp::udp_acceptor_config& config =
            transport::udp::udp_acceptor_config()) {
        server_config_.udp_acceptors.push_back(config);
        return *this;
    }

    /*!
     * \brief set to listen a UDP port
     *
     * \param ip_address IP address string (IPv4 or IPv6)
     * \param port port number
     * \param compression_type compression type
     * \return this object
     */
    server_builder& listen_udp(const std::string& ip_address,
        std::uint16_t port,
        transport::compression_type compression_type =
            transport::compression_type::none) {
        transport::udp::udp_acceptor_config config;
        config.host = ip_address;
        config.port = port;
        config.compression.type = compression_type;
        listen_udp(config);
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
        const auto threads = std::make_shared<thread_pool>(
            logger_, server_config_.num_threads.value());

        auto method_server = std::make_shared<execution::simple_method_server>(
            logger_, *threads, methods_);

        const std::size_t num_acceptors = server_config_.tcp_acceptors.size() +
            server_config_.udp_acceptors.size();
        if (num_acceptors == 0) {
            throw exception(
                error_info(error_code::invalid_config_value, "no acceptors"));
        }

        std::vector<std::shared_ptr<transport::acceptor>> acceptors;
        acceptors.reserve(num_acceptors);
        for (const auto& config : server_config_.tcp_acceptors) {
            acceptors.push_back(transport::tcp::create_tcp_acceptor(logger_,
                *threads,
                transport::create_compressor_factory(config.compression),
                transport::create_parser_factory(config.compression), config));
        }
        for (const auto& config : server_config_.udp_acceptors) {
            acceptors.push_back(transport::udp::create_udp_acceptor(logger_,
                *threads,
                transport::create_compressor_factory(config.compression),
                transport::create_parser_factory(config.compression), config));
        }

        auto srv = std::make_unique<server>(logger_, threads,
            std::move(acceptors), std::move(method_server), server_config_);
        srv->start();

        return srv;
    }

private:
    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_{
        logging::create_stdout_logger(mprpc::logging::log_level::info)};

    //! server configuration
    mprpc::server_config server_config_{};

    //! methods
    std::vector<std::shared_ptr<execution::method_executor>> methods_{};
};

}  // namespace mprpc

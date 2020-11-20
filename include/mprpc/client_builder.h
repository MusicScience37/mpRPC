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
 * \brief declaration and implementation of client_builder class
 */
#pragma once

#include <functional>
#include <memory>

#include "mprpc/client.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/transport.h"
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/connector.h"
#include "mprpc/transport/create_compressor_factory.h"
#include "mprpc/transport/create_parser_factory.h"
#include "mprpc/transport/tcp/tcp.h"
#include "mprpc/transport/udp/udp.h"

namespace mprpc {

/*!
 * \brief class to create client
 */
class client_builder {
public:
    //! construct
    client_builder() = default;

    /*!
     * \brief construct
     *
     * \param logger logger
     */
    explicit client_builder(std::shared_ptr<mprpc::logging::logger> logger)
        : logger_(std::move(logger)) {}

    /*!
     * \brief set number of threads
     *
     * \param num_threads number of threads
     * \return this object
     */
    client_builder& num_threads(std::size_t num_threads) {
        client_config_.num_threads = num_threads;
        return *this;
    }

    /*!
     * \brief set client configuration
     *
     * \param config configuration
     * \return this object
     */
    client_builder& client_config(mprpc::client_config config) {
        client_config_ = std::move(config);
        return *this;
    }

    /*!
     * \brief set to connect to a TCP port
     *
     * \param config configuration
     * \return this object
     */
    client_builder& connect_tcp(
        const transport::tcp::tcp_connector_config& config =
            transport::tcp::tcp_connector_config()) {
        client_config_.transport_type = "TCP";
        client_config_.tcp_connector = config;
        return *this;
    }

    /*!
     * \brief set to connect to a TCP port
     *
     * \param host host address to connect to
     * \param port port number to connect to
     * \param compression_type compression type
     * \return this object
     */
    client_builder& connect_tcp(const std::string& host, std::uint16_t port,
        transport::compression_type compression_type =
            transport::compression_type::none) {
        transport::tcp::tcp_connector_config config;
        config.host = host;
        config.port = port;
        config.compression.type = compression_type;
        connect_tcp(config);
        return *this;
    }

    /*!
     * \brief set to connect to a UDP port
     *
     * \param config configuration
     * \return this object
     */
    client_builder& connect_udp(
        const transport::udp::udp_connector_config& config =
            transport::udp::udp_connector_config()) {
        client_config_.transport_type = "UDP";
        client_config_.udp_connector = config;
        return *this;
    }

    /*!
     * \brief set to connect to a UDP port
     *
     * \param host host address to connect to
     * \param port port number to connect to
     * \param compression_type compression type
     * \return this object
     */
    client_builder& connect_udp(const std::string& host, std::uint16_t port,
        transport::compression_type compression_type =
            transport::compression_type::none) {
        transport::udp::udp_connector_config config;
        config.host = host;
        config.port = port;
        config.compression.type = compression_type;
        connect_udp(config);
        return *this;
    }

    /*!
     * \brief create client
     *
     * \return client
     */
    std::unique_ptr<client> create() {
        const auto threads = std::make_shared<thread_pool>(
            logger_, client_config_.num_threads.value());

        std::shared_ptr<transport::connector> connector;
        if (client_config_.transport_type.value() == "TCP") {
            connector = transport::tcp::create_tcp_connector(logger_, *threads,
                transport::create_compressor_factory(
                    client_config_.tcp_connector.compression),
                transport::create_parser_factory(
                    client_config_.tcp_connector.compression),
                client_config_.tcp_connector);
        } else {
            // UDP
            connector = transport::udp::create_udp_connector(logger_, *threads,
                transport::create_compressor_factory(
                    client_config_.udp_connector.compression),
                transport::create_parser_factory(
                    client_config_.udp_connector.compression),
                client_config_.udp_connector);
        }

        auto res = std::make_unique<client>(
            logger_, threads, std::move(connector), client_config_);
        res->start();

        return res;
    }

private:
    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_{
        logging::create_stdout_logger(mprpc::logging::log_level::warn)};

    //! client configuration
    mprpc::client_config client_config_{};
};

}  // namespace mprpc

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
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/transport.h"

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
        num_threads_ = num_threads;
        return *this;
    }

    /*!
     * \brief set to use compression with zstd library
     *
     * \param config configuration
     * \return this object
     */
    client_builder& use_zstd_compression(
        transport::compressors::zstd_compressor_config config =
            transport::compressors::zstd_compressor_config()) {
        compressor_factory_ =
            std::make_shared<transport::compressors::zstd_compressor_factory>(
                config);
        parser_factory_ =
            std::make_shared<transport::parsers::zstd_parser_factory>();
        return *this;
    }

    /*!
     * \brief set to connect to a TCP port
     *
     * \param host host address to connect to
     * \param port port number to connect to
     * \param config configuration
     * \return this object
     */
    client_builder& connect_tcp(const std::string& host, std::uint16_t port,
        transport::tcp::tcp_connector_config config =
            transport::tcp::tcp_connector_config()) {
        connector_factory_ =
            [host, port, config](
                const std::shared_ptr<mprpc::logging::logger>& logger,
                thread_pool& threads,
                const std::shared_ptr<transport::compressor_factory>&
                    comp_factory,
                const std::shared_ptr<transport::parser_factory>& factory) {
                return transport::tcp::create_tcp_connector(
                    logger, host, port, threads, comp_factory, factory, config);
            };
        return *this;
    }

    /*!
     * \brief set to connect to a UDP port
     *
     * \param host host address to connect to
     * \param port port number to connect to
     * \param config configuration
     * \return this object
     */
    client_builder& connect_udp(const std::string& host, std::uint16_t port,
        transport::udp::udp_connector_config config =
            transport::udp::udp_connector_config()) {
        connector_factory_ =
            [host, port, config](
                const std::shared_ptr<mprpc::logging::logger>& logger,
                thread_pool& threads,
                const std::shared_ptr<transport::compressor_factory>&
                    comp_factory,
                const std::shared_ptr<transport::parser_factory>& factory) {
                return transport::udp::create_udp_connector(
                    logger, host, port, threads, comp_factory, factory, config);
            };
        return *this;
    }

    /*!
     * \brief create client
     *
     * \return client
     */
    std::unique_ptr<client> create() {
        const auto threads =
            std::make_shared<thread_pool>(logger_, num_threads_);

        auto connector = connector_factory_(
            logger_, *threads, compressor_factory_, parser_factory_);

        auto res =
            std::make_unique<client>(logger_, threads, std::move(connector));
        res->start();

        return res;
    }

private:
    //! logger
    std::shared_ptr<mprpc::logging::logger> logger_{
        logging::create_stdout_logger(mprpc::logging::log_level::warn)};

    //! number of threads
    std::size_t num_threads_{1};

    //! compressor factory
    std::shared_ptr<transport::compressor_factory> compressor_factory_{
        std::make_shared<transport::compressors::null_compressor_factory>()};

    //! parser factory
    std::shared_ptr<transport::parser_factory> parser_factory_{
        std::make_shared<transport::parsers::msgpack_parser_factory>()};

    //! type of connector factory method
    using connector_factory_type =
        std::function<std::shared_ptr<transport::connector>(
            const std::shared_ptr<mprpc::logging::logger>&, thread_pool&,
            const std::shared_ptr<transport::compressor_factory>&,
            const std::shared_ptr<transport::parser_factory>&)>;

    //! connector factories
    connector_factory_type connector_factory_{};
};

}  // namespace mprpc

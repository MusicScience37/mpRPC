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
 * \brief declaration and implementation of option class
 */
#pragma once

#include <stdexcept>

#include <toml.hpp>

#include "mprpc/client_config.h"
#include "mprpc/config/option.h"
#include "mprpc/mprpc_config.h"
#include "mprpc/num_threads.h"
#include "mprpc/server_config.h"
#include "mprpc/transport/asio_helper/stream_socket_config.h"
#include "mprpc/transport/common/ip_common_config.h"
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/compressors/zstd_compressor_config.h"
#include "mprpc/transport/tcp/tcp_acceptor_config.h"
#include "mprpc/transport/tcp/tcp_connector_config.h"
#include "mprpc/transport/udp/udp_acceptor_config.h"
#include "mprpc/transport/udp/udp_connector_config.h"

namespace toml {

/*!
 * \brief toml::from specialization for mprpc::config::option
 *
 * \tparam OptionType option type
 */
template <typename OptionType>
struct from<mprpc::config::option<OptionType>> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::config::option<OptionType> from_toml(
        const basic_value<Comment, Table, Array>& value) {
        // NOLINTNEXTLINE: copy is necessary for some types
        const auto converted =
            toml::get<typename OptionType::value_type>(value);
        if (!OptionType::validate(converted)) {
            throw std::runtime_error(format_error("invalid value for " +
                    OptionType::name() + ", " + OptionType::description(),
                value, "invalid value"));
        }
        mprpc::config::option<OptionType> option;
        option = converted;
        return option;
    }
};

/*!
 * \brief toml::from specialization for mprpc::transport::compression_type
 */
template <>
struct from<mprpc::transport::compression_type> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::transport::compression_type from_toml(
        const basic_value<Comment, Table, Array>& value) {
        const auto& str = value.as_string();
        if (str == "none") {
            return mprpc::transport::compression_type::none;
        }
        if (str == "zstd") {
            return mprpc::transport::compression_type::zstd;
        }
        throw std::runtime_error(format_error(
            "invalid value for type, compression type (none, zstd)", value,
            "invalid type"));
    }
};

/*!
 * \brief toml::from specialization for mprpc::transport::compression_config
 */
template <>
struct from<mprpc::transport::compression_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::transport::compression_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::transport::compression_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "type") {
                config.type = toml::get<mprpc::config::option<
                    mprpc::transport::compression_type_type>>(pair.second);
            } else if (pair.first == "zstd_compression_level") {
                config.zstd_compression_level =
                    toml::get<mprpc::config::option<mprpc::transport::
                            compressors::zstd_compression_level_type>>(
                        pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

/*!
 * \brief toml::from specialization for
 * mprpc::transport::tcp::tcp_acceptor_config
 */
template <>
struct from<mprpc::transport::tcp::tcp_acceptor_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::transport::tcp::tcp_acceptor_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::transport::tcp::tcp_acceptor_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "compression") {
                config.compression =
                    toml::get<mprpc::transport::compression_config>(
                        pair.second);
            } else if (pair.first == "host") {
                config.host = toml::get<
                    mprpc::config::option<mprpc::transport::common::host_type>>(
                    pair.second);
            } else if (pair.first == "port") {
                config.port = toml::get<
                    mprpc::config::option<mprpc::transport::common::port_type>>(
                    pair.second);
            } else if (pair.first == "streaming_min_buf_size") {
                config.streaming_min_buf_size =
                    toml::get<mprpc::config::option<mprpc::transport::
                            asio_helper::streaming_min_buf_size_type>>(
                        pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

/*!
 * \brief toml::from specialization for
 * mprpc::transport::tcp::tcp_connector_config
 */
template <>
struct from<mprpc::transport::tcp::tcp_connector_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::transport::tcp::tcp_connector_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::transport::tcp::tcp_connector_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "compression") {
                config.compression =
                    toml::get<mprpc::transport::compression_config>(
                        pair.second);
            } else if (pair.first == "host") {
                config.host = toml::get<
                    mprpc::config::option<mprpc::transport::common::host_type>>(
                    pair.second);
            } else if (pair.first == "port") {
                config.port = toml::get<
                    mprpc::config::option<mprpc::transport::common::port_type>>(
                    pair.second);
            } else if (pair.first == "streaming_min_buf_size") {
                config.streaming_min_buf_size =
                    toml::get<mprpc::config::option<mprpc::transport::
                            asio_helper::streaming_min_buf_size_type>>(
                        pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

/*!
 * \brief toml::from specialization for
 * mprpc::transport::udp::udp_acceptor_config
 */
template <>
struct from<mprpc::transport::udp::udp_acceptor_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::transport::udp::udp_acceptor_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::transport::udp::udp_acceptor_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "compression") {
                config.compression =
                    toml::get<mprpc::transport::compression_config>(
                        pair.second);
            } else if (pair.first == "host") {
                config.host = toml::get<
                    mprpc::config::option<mprpc::transport::common::host_type>>(
                    pair.second);
            } else if (pair.first == "port") {
                config.port = toml::get<
                    mprpc::config::option<mprpc::transport::common::port_type>>(
                    pair.second);
            } else if (pair.first == "datagram_buf_size") {
                config.datagram_buf_size = toml::get<mprpc::config::option<
                    mprpc::transport::udp::datagram_buf_size_type>>(
                    pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

/*!
 * \brief toml::from specialization for
 * mprpc::transport::udp::udp_connector_config
 */
template <>
struct from<mprpc::transport::udp::udp_connector_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::transport::udp::udp_connector_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::transport::udp::udp_connector_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "compression") {
                config.compression =
                    toml::get<mprpc::transport::compression_config>(
                        pair.second);
            } else if (pair.first == "host") {
                config.host = toml::get<
                    mprpc::config::option<mprpc::transport::common::host_type>>(
                    pair.second);
            } else if (pair.first == "port") {
                config.port = toml::get<
                    mprpc::config::option<mprpc::transport::common::port_type>>(
                    pair.second);
            } else if (pair.first == "datagram_buf_size") {
                config.datagram_buf_size = toml::get<mprpc::config::option<
                    mprpc::transport::udp::datagram_buf_size_type>>(
                    pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

/*!
 * \brief toml::from specialization for mprpc::server_config
 */
template <>
struct from<mprpc::server_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::server_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::server_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "num_threads") {
                config.num_threads =
                    toml::get<mprpc::config::option<mprpc::num_threads_type>>(
                        pair.second);
            } else if (pair.first == "tcp_acceptors") {
                config.tcp_acceptors = toml::get<
                    std::vector<mprpc::transport::tcp::tcp_acceptor_config>>(
                    pair.second);
            } else if (pair.first == "udp_acceptors") {
                config.udp_acceptors = toml::get<
                    std::vector<mprpc::transport::udp::udp_acceptor_config>>(
                    pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

/*!
 * \brief toml::from specialization for mprpc::transport_type
 */
template <>
struct from<mprpc::transport_type> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::transport_type from_toml(
        const basic_value<Comment, Table, Array>& value) {
        const auto& str = value.as_string();
        if ((str == "tcp") || (str == "TCP")) {
            return mprpc::transport_type::tcp;
        }
        if ((str == "udp") || (str == "UDP")) {
            return mprpc::transport_type::udp;
        }
        throw std::runtime_error(
            format_error("invalid value for transport type (TCP, UDP)", value,
                "invalid type"));
    }
};

/*!
 * \brief toml::from specialization for mprpc::client_config
 */
template <>
struct from<mprpc::client_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::client_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::client_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "num_threads") {
                config.num_threads =
                    toml::get<mprpc::config::option<mprpc::num_threads_type>>(
                        pair.second);
            } else if (pair.first == "sync_request_timeout_ms") {
                config.sync_request_timeout_ms = toml::get<
                    mprpc::config::option<mprpc::sync_request_timeout_ms_type>>(
                    pair.second);
            } else if (pair.first == "connector_type") {
                config.connector_type = toml::get<
                    mprpc::config::option<mprpc::connector_type_type>>(
                    pair.second);
            } else if (pair.first == "tcp_connector") {
                config.tcp_connector =
                    toml::get<mprpc::transport::tcp::tcp_connector_config>(
                        pair.second);
            } else if (pair.first == "udp_connector") {
                config.udp_connector =
                    toml::get<mprpc::transport::udp::udp_connector_config>(
                        pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

/*!
 * \brief toml::from specialization for mprpc::mprpc_config
 */
template <>
struct from<mprpc::mprpc_config> {
    /*!
     * \brief convert from toml value
     *
     * \tparam Comment comment type
     * \tparam Table table type
     * \tparam Array array type
     * \param value toml value
     * \return converted value
     */
    template <typename Comment, template <typename...> class Table,
        template <typename...> class Array>
    static mprpc::mprpc_config from_toml(
        const basic_value<Comment, Table, Array>& value) {
        mprpc::mprpc_config config;
        for (const auto& pair : value.as_table()) {
            if (pair.first == "client") {
                config.client = toml::get<mprpc::client_config>(pair.second);
            } else if (pair.first == "server") {
                config.server = toml::get<mprpc::server_config>(pair.second);
            } else {
                throw std::runtime_error(format_error(
                    "invalid key " + pair.first, value, "invalid key exists"));
            }
        }
        return config;
    }
};

}  // namespace toml

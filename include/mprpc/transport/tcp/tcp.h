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
 * \brief declaration of TCP-related factory functions
 */
#pragma once

#include <memory>

#include "mprpc/logging/labeled_logger.h"
#include "mprpc/mprpc_export.h"
#include "mprpc/thread_pool.h"
#include "mprpc/transport/acceptor.h"
#include "mprpc/transport/compressor.h"
#include "mprpc/transport/connector.h"
#include "mprpc/transport/parser.h"
#include "mprpc/transport/tcp/tcp_acceptor_config.h"
#include "mprpc/transport/tcp/tcp_connector_config.h"

namespace mprpc {
namespace transport {
namespace tcp {

/*!
 * \brief create a TCP acceptor
 *
 * \param logger logger
 * \param threads thread pool
 * \param comp_factory compressor factory
 * \param parser_factory_ptr parser factory
 * \param config configuration
 * \return TCP acceptor
 */
MPRPC_EXPORT std::shared_ptr<acceptor> create_tcp_acceptor(
    const logging::labeled_logger& logger, thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const tcp_acceptor_config& config);

/*!
 * \brief create a TCP connector
 *
 * \param logger logger
 * \param threads thread pool
 * \param comp_factory compressor factory
 * \param parser_factory_ptr parser factory
 * \param config configuration
 * \return TCP connector
 */
MPRPC_EXPORT std::shared_ptr<connector> create_tcp_connector(
    const logging::labeled_logger& logger, thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const tcp_connector_config& config);

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc

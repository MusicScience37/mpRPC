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
 * \brief declaration of functions for UDP
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
#include "mprpc/transport/udp/udp_acceptor_config.h"
#include "mprpc/transport/udp/udp_connector_config.h"

namespace mprpc {
namespace transport {
namespace udp {

/*!
 * \brief create a UDP acceptor
 *
 * \param logger logger
 * \param threads thread pool
 * \param comp_factory compressor factory
 * \param parser_factory_ptr parser factory
 * \param config configuration
 * \return UDP acceptor
 */
MPRPC_EXPORT std::shared_ptr<acceptor> create_udp_acceptor(
    logging::labeled_logger logger, thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const udp_acceptor_config& config);

/*!
 * \brief create a UDP connector
 *
 * \param logger logger
 * \param threads thread pool
 * \param comp_factory compressor factory
 * \param parser_factory_ptr parser factory
 * \param config configuration
 * \return UDP connector
 */
MPRPC_EXPORT std::shared_ptr<connector> create_udp_connector(
    logging::labeled_logger logger, thread_pool& threads,
    const std::shared_ptr<compressor_factory>& comp_factory,
    const std::shared_ptr<parser_factory>& parser_factory_ptr,
    const udp_connector_config& config);

}  // namespace udp
}  // namespace transport
}  // namespace mprpc

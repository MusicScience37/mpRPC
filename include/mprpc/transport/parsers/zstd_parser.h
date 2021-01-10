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
 * \brief declaration and implementation of zstd-based parsers
 */
#pragma once

#include "mprpc/logging/logger.h"
#include "mprpc/mprpc_export.h"
#include "mprpc/transport/parser.h"

namespace mprpc {
namespace transport {
namespace parsers {

/*!
 * \brief create parser of messages using zstd library without streaming
 *
 * \param logger logger
 * \return parser
 */
MPRPC_EXPORT std::unique_ptr<parser> create_zstd_parser(
    logging::labeled_logger logger);

/*!
 * \brief create parser of messages using zstd library with streaming
 *
 * \param logger logger
 * \return parser
 */
MPRPC_EXPORT std::unique_ptr<streaming_parser> create_zstd_streaming_parser(
    const logging::labeled_logger& logger);

/*!
 * \brief class of factories of parsers using zstd library
 */
class zstd_parser_factory final : public parser_factory {
public:
    //! \copydoc mprpc::transport::parser_factory::create_parser
    std::unique_ptr<parser> create_parser(
        logging::labeled_logger logger) override {
        return create_zstd_parser(std::move(logger));
    }

    //! \copydoc mprpc::transport::parser_factory::create_streaming_parser
    std::unique_ptr<streaming_parser> create_streaming_parser(
        logging::labeled_logger logger) override {
        return create_zstd_streaming_parser(logger);
    }

    //! construct
    zstd_parser_factory() = default;
};

}  // namespace parsers
}  // namespace transport
}  // namespace mprpc

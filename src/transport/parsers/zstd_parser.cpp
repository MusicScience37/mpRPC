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
 * \brief implementation of zstd-based parsers
 */
#include "mprpc/transport/parsers/zstd_parser.h"

#include "mprpc/transport/parsers/impl/zstd_parser_impl.h"

namespace mprpc {
namespace transport {
namespace parsers {

std::unique_ptr<parser> create_zstd_parser(
    std::shared_ptr<logging::logger> logger) {
    return std::make_unique<impl::zstd_parser>(std::move(logger));
}

std::unique_ptr<streaming_parser> create_zstd_streaming_parser(
    const std::shared_ptr<logging::logger>& logger) {
    return std::make_unique<impl::zstd_streaming_parser>(logger);
}

}  // namespace parsers
}  // namespace transport
}  // namespace mprpc

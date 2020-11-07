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
 * \brief implementation of parsers using msgpack-c library
 */
#include "mprpc/transport/parsers/msgpack_parser.h"

#include "mprpc/transport/parsers/impl/msgpack_parser_impl.h"

namespace mprpc {
namespace transport {
namespace parsers {

std::unique_ptr<parser> create_msgpack_parser(
    std::shared_ptr<logging::logger> /*logger*/) {  // NOLINT: for compatible
                                                    // API
    return std::make_unique<impl::msgpack_parser>();
}

std::unique_ptr<streaming_parser> create_msgpack_streaming_parser(
    std::shared_ptr<logging::logger> logger) {
    return std::make_unique<impl::msgpack_streaming_parser>(std::move(logger));
}

}  // namespace parsers
}  // namespace transport
}  // namespace mprpc

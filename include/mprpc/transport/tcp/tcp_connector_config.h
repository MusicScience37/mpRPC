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
 * \brief declaration and implementation of tcp_connector_config class
 */
#pragma once

#include "mprpc/transport/asio_helper/stream_socket_config.h"
#include "mprpc/transport/common/ip_common_config.h"

namespace mprpc {
namespace transport {
namespace tcp {

/*!
 * \brief configuration of TCP connector
 */
struct tcp_connector_config : asio_helper::stream_socket_config,
                              common::ip_common_config {};

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc

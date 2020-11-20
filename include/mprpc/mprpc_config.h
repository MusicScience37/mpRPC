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
 * \brief declaration and implementation of mprpc_config class
 */
#pragma once

#include "mprpc/client_config.h"
#include "mprpc/server_config.h"

namespace mprpc {

/*!
 * \brief configuration of this library
 */
struct mprpc_config {
    //! configuration of clients
    client_config client;

    //! configuration of servers
    server_config server;
};

}  // namespace mprpc

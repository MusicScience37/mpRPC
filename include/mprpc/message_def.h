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
 * \brief definition for messages
 */
#pragma once

#include <cstdint>

#include <msgpack.hpp>

namespace mprpc {

//! enumeration of message types
enum class msgtype {
    request = 0,      //!< request
    response = 1,     //!< response
    notification = 2  //!< notification
};

//! type of message IDs
using msgid_t = std::uint32_t;

}  // namespace mprpc

#ifndef MPRPC_DOCUMENTATION
MSGPACK_ADD_ENUM(mprpc::msgtype);
#endif

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
 * \brief basic definition of error codes
 */
#pragma once

#include <cstdint>

namespace mprpc {

/*!
 * \brief type of error codes
 */
using error_code_t = std::uint32_t;

/*!
 * \brief namespace of error codes
 */
namespace error_code {

/*!
 * \brief enumeration of common error codes
 */
enum common_errors : error_code_t {
    success = 0,      //!< success (no error)
    unexpected_error  //!< unexpected error
};

/*!
 * \brief enumeration of message-related error codes
 */
enum message_errors : error_code_t {
    parse_error = 100,  //!< parse error
    invalid_message     //!< invalid message
};

/*!
 * \brief enumeration of network-related error codes
 */
enum network_errors : error_code_t {
    eof = 200,          //!< end of file
    failed_to_listen,   //!< failed to listen to an address
    failed_to_accept,   //!< failed to accept a connection
    failed_to_resolve,  //!< failed to resolve a server
    failed_to_connect,  //!< failed to connect to a server
    failed_to_read,     //!< failed to read data
    failed_to_write     //!< failed to write data
};

/*!
 * \brief enumeration of execution-related error codes
 */
enum execution_errors : error_code_t {
    method_not_found = 300  //!< method not found
};

/*!
 * \brief enumeration of future-related error codes
 */
enum future_errors : error_code_t {
    invalid_future_use = 400  //!< use of invalid future
};

/*!
 * \brief enumeration of configuration-related error codes
 */
enum config_errors : error_code_t {
    invalid_config_value = 500,  //!< invalid configuration value
    config_parse_error           //!< parse error of configuration files
};

/*!
 * \brief enumeration of client-specific error codes
 */
enum client_errors : error_code_t {
    timeout = 600  //!< timeout
};

}  // namespace error_code

}  // namespace mprpc

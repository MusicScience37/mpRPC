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
 * \brief declaration and implementation of tcp_address class
 */
#pragma once

#include <sstream>

#include <asio/ip/tcp.hpp>

#include "mprpc/transport/address.h"

namespace mprpc {
namespace transport {
namespace tcp {

/*!
 * \brief class of addresses for TCP sockets
 */
class tcp_address : public address {
public:
    /*!
     * \brief construct
     *
     * \param endpoint endpoint
     */
    explicit tcp_address(asio::ip::tcp::endpoint endpoint)
        : endpoint_(std::move(endpoint)) {}

    //! \copydoc mprpc::transport::address::full_address
    std::string full_address() const override {
        std::ostringstream stream;
        stream << endpoint_;
        return stream.str();
    }

private:
    //! endpoint
    asio::ip::tcp::endpoint endpoint_;
};

}  // namespace tcp
}  // namespace transport
}  // namespace mprpc

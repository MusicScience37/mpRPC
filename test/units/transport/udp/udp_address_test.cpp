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
 * \brief test of udp_address class
 */
#include "mprpc/transport/udp/impl/udp_address.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("mprpc::transport::udp::udp_address") {
    SECTION("IP v4 address") {
        const auto address = asio::ip::address_v4::from_string("192.0.2.0");
        constexpr std::uint16_t port = 12345;
        const auto endpoint = asio::ip::udp::endpoint(address, port);

        const auto udp_address =
            std::make_shared<mprpc::transport::udp::impl::udp_address>(
                endpoint);
        REQUIRE(udp_address->full_address() == "192.0.2.0:12345");
    }

    SECTION("IP v6 address") {
        const auto address = asio::ip::address_v6::from_string(
            "2001:db8:85a3:0:0:8a2e:370:7334");
        constexpr std::uint16_t port = 4321;
        const auto endpoint = asio::ip::udp::endpoint(address, port);

        const auto udp_address =
            std::make_shared<mprpc::transport::udp::impl::udp_address>(
                endpoint);
        REQUIRE(udp_address->full_address() ==
            "[2001:db8:85a3::8a2e:370:7334]:4321");
    }
}

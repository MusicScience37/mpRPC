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
 * \brief test of compression and decompression using zstd library with
 * streaming
 */
#include <catch2/catch_test_macros.hpp>

#include "../../create_logger.h"
#include "mprpc/transport/compressors/zstd_compressor.h"
#include "mprpc/transport/parsers/zstd_parser.h"

TEST_CASE("mprpc::transport::zstd with streaming") {
    const auto logger = create_logger("mprpc::transport::zstd with streaming");

    mprpc::transport::compression_config compressor_config;
    const auto compressor = std::make_shared<
        mprpc::transport::compressors::zstd_compressor_factory>(
        compressor_config)
                                ->create_streaming_compressor(logger);

    const auto parser =
        std::make_shared<mprpc::transport::parsers::zstd_parser_factory>()
            ->create_streaming_parser(logger);

    SECTION("compress and decompress") {
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        compressor->init();

        const auto compressed_data = compressor->compress(data);
        REQUIRE(compressed_data != data);

        parser->prepare_buffer(compressed_data.size());
        std::copy(compressed_data.data(),
            compressed_data.data() + compressed_data.size(), parser->buffer());
        REQUIRE(parser->parse_next(compressed_data.size()));
        REQUIRE(parser->get() == data);
    }

    SECTION("compress and decompress twice") {
        const auto data_str1 =
            std::string({char(0x92), char(0x01), char(0x02)});
        auto data = mprpc::message_data(data_str1.data(), data_str1.size());

        compressor->init();

        auto compressed_data = compressor->compress(data);
        REQUIRE(compressed_data != data);

        parser->prepare_buffer(compressed_data.size());
        std::copy(compressed_data.data(),
            compressed_data.data() + compressed_data.size(), parser->buffer());
        REQUIRE(parser->parse_next(compressed_data.size()));
        REQUIRE(parser->get() == data);

        constexpr std::size_t data_str2_size = 30;
        const auto data_str2 = std::string({char(0xDC), char(0x00),
                                   static_cast<char>(data_str2_size)}) +
            std::string(data_str2_size, char(0x01));
        data = mprpc::message_data(data_str2.data(), data_str2.size());

        compressed_data = compressor->compress(data);
        REQUIRE(compressed_data != data);

        parser->prepare_buffer(compressed_data.size());
        std::copy(compressed_data.data(),
            compressed_data.data() + compressed_data.size(), parser->buffer());
        REQUIRE(parser->parse_next(compressed_data.size()));
        REQUIRE(parser->get() == data);
    }

    SECTION("compress and decompress separate data") {
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        compressor->init();

        const auto compressed_data = compressor->compress(data);
        REQUIRE(compressed_data != data);

        constexpr std::size_t first_data_size = 4;
        REQUIRE(compressed_data.size() > first_data_size);

        parser->prepare_buffer(compressed_data.size());
        std::copy(compressed_data.data(),
            compressed_data.data() + first_data_size, parser->buffer());
        REQUIRE_FALSE(parser->parse_next(first_data_size));

        parser->prepare_buffer(compressed_data.size());
        std::copy(compressed_data.data() + first_data_size,
            compressed_data.data() + compressed_data.size(), parser->buffer());
        REQUIRE(parser->parse_next(compressed_data.size() - first_data_size));
        REQUIRE(parser->get() == data);
    }
}

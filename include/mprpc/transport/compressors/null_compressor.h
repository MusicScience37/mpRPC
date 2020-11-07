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
 * \brief declaration and implementation of null_compressor classes
 */
#pragma once

#include "mprpc/transport/compressor.h"

namespace mprpc {
namespace transport {
namespace compressors {

/*!
 * \brief class of compressor without compression
 */
class null_compressor final : public compressor {
public:
    //! \copydoc mprpc::transport::compressor::compress
    message_data compress(message_data data) override { return data; }
};

/*!
 * \brief class of compressor without compression
 */
class null_streaming_compressor final : public streaming_compressor {
public:
    //! \copydoc mprpc::transport::streaming_compressor::init
    void init() override {}

    //! \copydoc mprpc::transport::streaming_compressor::compress
    message_data compress(message_data data) override { return data; }
};

/*!
 * \brief class of factories of compressors without compression
 */
class null_compressor_factory final : public compressor_factory {
public:
    /*!
     * \brief create a compressor without streaming
     *
     * \return compressor
     */
    std::unique_ptr<compressor> create_compressor(
        std::shared_ptr<logging::logger> /*logger*/) override {
        return std::make_unique<null_compressor>();
    }

    /*!
     * \brief create a compressor with streaming
     *
     * \return compressor
     */
    std::unique_ptr<streaming_compressor> create_streaming_compressor(
        std::shared_ptr<logging::logger> /*logger*/) override {
        return std::make_unique<null_streaming_compressor>();
    }
};

}  // namespace compressors
}  // namespace transport
}  // namespace mprpc

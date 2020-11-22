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
 * \brief declaration and implementation of compressor classes
 */
#pragma once

#include <memory>

#include "mprpc/exception.h"
#include "mprpc/logging/labeled_logger.h"
#include "mprpc/message_data.h"
#include "mprpc/shared_binary.h"

namespace mprpc {
namespace transport {

/*!
 * \brief base class of compressors of messages
 */
class compressor {
public:
    /*!
     * \brief compress data
     *
     * \param data data
     * \return compressed data
     */
    virtual shared_binary compress(message_data data) = 0;

    //! construct
    compressor() noexcept = default;

    //! destruct
    virtual ~compressor() = default;

    compressor(const compressor&) = delete;
    compressor(compressor&&) = delete;
    compressor& operator=(const compressor&) = delete;
    compressor& operator=(compressor&&) = delete;
};

/*!
 * \brief base class of compressors of message with streaming
 */
class streaming_compressor {
public:
    /*!
     * \brief initialize
     */
    virtual void init() = 0;

    /*!
     * \brief compress data
     *
     * \param data data
     * \return compressed data
     */
    virtual shared_binary compress(message_data data) = 0;

    //! construct
    streaming_compressor() noexcept = default;

    //! destruct
    virtual ~streaming_compressor() = default;

    streaming_compressor(const streaming_compressor&) = delete;
    streaming_compressor(streaming_compressor&&) = delete;
    streaming_compressor& operator=(const streaming_compressor&) = delete;
    streaming_compressor& operator=(streaming_compressor&&) = delete;
};

/*!
 * \brief base class of factories of compressors
 */
class compressor_factory {
public:
    /*!
     * \brief create a compressor without streaming
     *
     * \param logger logger
     * \return compressor
     */
    virtual std::unique_ptr<compressor> create_compressor(
        logging::labeled_logger logger) = 0;

    /*!
     * \brief create a compressor with streaming
     *
     * \param logger logger
     * \return compressor
     */
    virtual std::unique_ptr<streaming_compressor> create_streaming_compressor(
        logging::labeled_logger logger) = 0;

    //! construct
    compressor_factory() noexcept = default;

    //! destruct
    virtual ~compressor_factory() = default;

    compressor_factory(const compressor_factory&) = delete;
    compressor_factory(compressor_factory&&) = delete;
    compressor_factory& operator=(const compressor_factory&) = delete;
    compressor_factory& operator=(compressor_factory&&) = delete;
};

}  // namespace transport
}  // namespace mprpc

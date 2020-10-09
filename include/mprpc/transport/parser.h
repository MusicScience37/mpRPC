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
 * \brief declaration and implementation of parser and streaming_parser class
 */
#pragma once

#include <memory>

#include "mprpc/exception.h"
#include "mprpc/message_data.h"

namespace mprpc {
namespace transport {

/*!
 * \brief base class of parsers of messages
 */
class parser {
public:
    /*!
     * \brief parse data
     *
     * \param data data
     * \param size data size
     * \return message data
     *
     * \throw mprpc::exception on parse error
     */
    virtual message_data parse(const char* data, std::size_t size) = 0;

    //! construct
    parser() noexcept = default;

    //! destruct
    virtual ~parser() = default;

    parser(const parser&) = delete;
    parser(parser&&) = delete;
    parser& operator=(const parser&) = delete;
    parser& operator=(parser&&) = delete;
};

/*!
 * \brief base class of parsers of message with streaming
 */
class streaming_parser {
public:
    /*!
     * \brief prepare buffer with size
     *
     * \param size buffer size
     */
    virtual void prepare_buffer(std::size_t size) = 0;

    /*!
     * \brief get pointer to the buffer
     *
     * \return pointer to the buffer
     */
    virtual char* buffer() = 0;

    /*!
     * \brief set some bytes in the buffer to be consumed
     *
     * \param consumed_buf_size consumed buffer size
     */
    virtual void consumed(std::size_t consumed_buf_size) = 0;

    /*!
     * \brief parse next message
     *
     * \param consumed_buf_size size of consumed buffer
     * \return whether a message is parsed
     *
     * \throw mprpc::exception on parse error
     */
    virtual bool parse_next(std::size_t consumed_buf_size) = 0;

    /*!
     * \brief get parsed message
     *
     * \return parsed message
     */
    virtual message_data get() = 0;

    //! construct
    streaming_parser() noexcept = default;

    //! destruct
    virtual ~streaming_parser() = default;

    streaming_parser(const streaming_parser&) = delete;
    streaming_parser(streaming_parser&&) = delete;
    streaming_parser& operator=(const streaming_parser&) = delete;
    streaming_parser& operator=(streaming_parser&&) = delete;
};

/*!
 * \brief base class of factories of parsers
 */
class parser_factory {
public:
    /*!
     * \brief create a parser without streaming
     *
     * \param logger logger
     * \return parser
     */
    virtual std::unique_ptr<parser> create_parser(
        std::shared_ptr<logging::logger> logger) = 0;

    /*!
     * \brief create a parser with streaming
     *
     * \param logger logger
     * \return parser
     */
    virtual std::unique_ptr<streaming_parser> create_streaming_parser(
        std::shared_ptr<logging::logger> logger) = 0;

    //! construct
    parser_factory() noexcept = default;

    //! destruct
    virtual ~parser_factory() = default;

    parser_factory(const parser_factory&) = delete;
    parser_factory(parser_factory&&) = delete;
    parser_factory& operator=(const parser_factory&) = delete;
    parser_factory& operator=(parser_factory&&) = delete;
};

}  // namespace transport
}  // namespace mprpc

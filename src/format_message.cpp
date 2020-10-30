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
 * \brief implementation of format_message function
 */
#include "mprpc/format_message.h"

#include <fmt/core.h>
#include <msgpack.hpp>

namespace mprpc {
namespace impl {

/*!
 * \brief format binary to hex
 *
 * \param ptr pointer to the data
 * \param size data size
 * \param limit limit of output
 * \return std::string formatted binary
 */
inline std::string format_hex(
    const char* ptr, std::size_t size, std::size_t limit) {
    // NOLINTNEXTLINE: for readability
    constexpr char hex[] = "0123456789ABCDEF";

    std::string res;
    const std::size_t proccess_bytes = std::min<std::size_t>(size, limit / 2);

    for (std::size_t i = 0; i < proccess_bytes; ++i) {
        // NOLINTNEXTLINE: for readability
        const unsigned int value = static_cast<unsigned char>(ptr[i]);
        const unsigned int upper = value >> 4U;
        // NOLINTNEXTLINE: for readability
        res += hex[upper];
        const unsigned int lower = value & 0b00001111U;
        // NOLINTNEXTLINE: for readability
        res += hex[lower];
    }

    return res;
}

/*!
 * \brief class of visitors for formatting data
 */
class formatting_visitor : public msgpack::null_visitor {
private:
    //! formatted message
    std::string& msg_;

    //! limit of output length
    std::size_t len_limit_;

public:
    /*!
     * \brief construct
     *
     * \param formatted_message buffer of formatted message
     * \param length_limit limit of output length
     */
    formatting_visitor(std::string& formatted_message, std::size_t length_limit)
        : msg_(formatted_message), len_limit_(length_limit) {}

    /*!
     * \brief visit nil
     *
     * \return bool whether to continue
     */
    bool visit_nil() {
        msg_ += "nil";
        return check_length();
    }

    /*!
     * \brief visit boolean
     *
     * \param value value
     * \return bool whether to continue
     */
    bool visit_boolean(bool value) {
        if (value) {
            msg_ += "true";
        } else {
            msg_ += "false";
        }
        return check_length();
    }

    /*!
     * \brief visit positive interger
     *
     * \param value value
     * \return bool whether to continue
     */
    bool visit_positive_integer(std::uint64_t value) {
        msg_ += fmt::format("{}", value);
        return check_length();
    }

    /*!
     * \brief visit negative interger
     *
     * \param value value
     * \return bool whether to continue
     */
    bool visit_negative_integer(std::int64_t value) {
        msg_ += fmt::format("{}", value);
        return check_length();
    }

    /*!
     * \brief visit float32
     *
     * \param value value
     * \return bool whether to continue
     */
    bool visit_float32(float value) {
        msg_ += fmt::format("{:.6}F", value);
        return check_length();
    }

    /*!
     * \brief visit float64
     *
     * \param value value
     * \return bool whether to continue
     */
    bool visit_float64(double value) {
        msg_ += fmt::format("{:.12}", value);
        return check_length();
    }

    /*!
     * \brief visit string
     *
     * \param ptr pointer
     * \param size size
     * \return bool whether to continue
     */
    bool visit_str(const char* ptr, std::uint32_t size) {
        if (msg_.size() + size * 2 > len_limit_) {
            msg_ += " ...";
            return false;
        }
        msg_ += '"';
        msg_.append(ptr, size);
        msg_ += '"';
        return check_length();
    }

    /*!
     * \brief visit binary
     *
     * \param ptr pointer
     * \param size size
     * \return bool whether to continue
     */
    bool visit_bin(const char* ptr, std::uint32_t size) {
        if (msg_.size() + size * 2 > len_limit_) {
            msg_ += " ...";
            return false;
        }
        msg_ += "hex(";
        msg_ += format_hex(ptr, size, len_limit_);
        msg_ += ")";
        return check_length();
    }

    /*!
     * \brief visit extension
     *
     * \param ptr pointer
     * \param size size
     * \return bool whether to continue
     */
    bool visit_ext(const char* ptr, std::uint32_t size) {
        if (msg_.size() + size * 2 > len_limit_) {
            msg_ += " ...";
            return false;
        }
        msg_ += "ext(";
        msg_ += format_hex(ptr, size, len_limit_);
        msg_ += ")";
        return check_length();
    }

    /*!
     * \brief visit start of an array
     *
     * \return bool whether to continue
     */
    bool start_array(std::uint32_t /*num_elements*/) {
        msg_ += '[';
        return check_length();
    }

    /*!
     * \brief visit end of an array item
     *
     * \return bool whether to continue
     */
    bool end_array_item() {
        msg_ += ',';
        return check_length();
    }

    /*!
     * \brief visit end of an array
     *
     * \return bool whether to continue
     */
    bool end_array() {
        if (msg_.back() == ',') {
            msg_.pop_back();
        }
        msg_ += ']';
        return check_length();
    }

    /*!
     * \brief visit a map
     *
     * \return bool whether to continue
     */
    bool start_map(uint32_t /*num_kv_pairs*/) {
        msg_ += '{';
        return check_length();
    }

    /*!
     * \brief visit end of a key in maps
     *
     * \return bool whether to continue
     */
    bool end_map_key() {
        msg_ += ':';
        return check_length();
    }

    /*!
     * \brief visit end of a value in maps
     *
     * \return bool whether to continue
     */
    bool end_map_value() {
        msg_ += ',';
        return check_length();
    }

    /*!
     * \brief visit end of a map
     *
     * \return bool whether to continue
     */
    bool end_map() {
        if (msg_.back() == ',') {
            msg_.pop_back();
        }
        msg_ += '}';
        return check_length();
    }

    /*!
     * \brief report a parse error
     */
    void parse_error(  // NOLINT: use of external library
        size_t /*parsed_offset*/, size_t /*error_offset*/) {
        throw msgpack::parse_error("parse error");
    }

    /*!
     * \brief report an error of insufficient bytes
     */
    void insufficient_bytes(  // NOLINT: use of external library
        size_t /*parsed_offset*/, size_t /*error_offset*/) {
        throw msgpack::parse_error("parse error");
    }

private:
    /*!
     * \brief check output length
     *
     * \return bool whether to continue
     */
    bool check_length() {
        if (msg_.size() + 4 >= len_limit_) {
            msg_ += " ...";
            return false;
        }
        return true;
    }
};

}  // namespace impl

std::string format_message(const message_data& data,
    std::size_t length_limit) noexcept {
    try {
        std::string formatted_data;
        impl::formatting_visitor visitor(formatted_data, length_limit);
        msgpack::parse(data.data(), data.size(), visitor);
        return formatted_data;
    } catch (const msgpack::parse_error& /*error*/) {
        constexpr std::size_t invalid_data_str_size = 14;
        const std::size_t limit =
            std::max(length_limit, std::size_t(invalid_data_str_size)) -
            invalid_data_str_size;
        return "invalid_data(" +
            impl::format_hex(data.data(), data.size(), limit) + ")";
    }
}

}  // namespace mprpc

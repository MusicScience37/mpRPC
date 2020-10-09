#pragma once

#include <msgpack.hpp>

#include "mprpc/generate_string.h"
#include "mprpc/message_data.h"

namespace mprpc {

/*!
 * \brief generate message data of random string
 *
 * \param size size of string
 * \return message data
 */
inline message_data generate_string_data(std::size_t size) {
    const auto str = generate_string(size);
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, str);
    return message_data(buffer.data(), buffer.size());
}

}  // namespace mprpc

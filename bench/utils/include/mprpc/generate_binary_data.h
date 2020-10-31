#pragma once

#include <msgpack.hpp>

#include "mprpc/generate_binary.h"
#include "mprpc/message_data.h"

namespace mprpc {

/*!
 * \brief generate message data of random binary
 *
 * \param size size of binary
 * \return message data
 */
inline message_data generate_binary_data(std::size_t size) {
    const auto data = generate_binary(size);
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, data);
    return message_data(buffer.data(), buffer.size());
}

}  // namespace mprpc

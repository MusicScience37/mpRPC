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
 * \brief declaration and implementation of message_data class
 */
#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace mprpc {

/*!
 * \brief class of message data
 */
class message_data {
public:
    /*!
     * \brief construct
     *
     * \param data pointer to the message data
     * \param size size of the message data
     */
    message_data(const char* data, std::size_t size)
        : data_(std::make_shared<std::vector<char>>(data, data + size)) {}

    /*!
     * \brief get pointer to the message data
     *
     * \return pointer to the message data
     */
    const char* data() const noexcept { return data_->data(); }

    /*!
     * \brief get size of the message data
     *
     * \return size of the message data
     */
    std::size_t size() const noexcept { return data_->size(); }

private:
    //! message data
    std::shared_ptr<std::vector<char>> data_;
};

/*!
 * \brief compare two messages
 *
 * \param left left-hand-side object
 * \param right right-hand-side object
 * \return whether two messages are same
 */
inline bool operator==(const message_data& left, const message_data& right) {
    return std::equal(left.data(), left.data() + left.size(), right.data(),
        right.data() + right.size());
}

/*!
 * \brief compare two messages
 *
 * \param left left-hand-side object
 * \param right right-hand-side object
 * \return whether two messages are distinct
 */
inline bool operator!=(const message_data& left, const message_data& right) {
    return !operator==(left, right);
}

}  // namespace mprpc

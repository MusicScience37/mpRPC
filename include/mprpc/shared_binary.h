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
 * \brief declaration of shared_binary class
 */
#pragma once

#include <cstddef>

#include "mprpc/mprpc_export.h"

namespace mprpc {

/*!
 * \brief class of binary data with reference counts
 */
class MPRPC_EXPORT shared_binary {
public:
    /*!
     * \brief construct empty data
     */
    shared_binary() noexcept;

    /*!
     * \brief construct
     *
     * \param data pointer to the binary data
     * \param size size of the binary data
     */
    shared_binary(const char* data, std::size_t size);

    /*!
     * \brief copy
     *
     * \param data another data
     */
    shared_binary(const shared_binary& data) noexcept;

    /*!
     * \brief move
     *
     * \param data another data
     */
    shared_binary(shared_binary&& data) noexcept;

    /*!
     * \brief copy
     *
     * \param data another data
     * \return this object
     */
    shared_binary& operator=(const shared_binary& data) noexcept;

    /*!
     * \brief move
     *
     * \param data another data
     * \return this object
     */
    shared_binary& operator=(shared_binary&& data) noexcept;

    //! destruct
    ~shared_binary();

    /*!
     * \brief check whether this object has data
     *
     * \return
     */
    bool has_data() const noexcept;

    /*!
     * \brief get binary data
     *
     * \note This won't check whether this object has data
     *
     * \return binary data
     */
    const char* data() const noexcept;

    /*!
     * \brief get size of the binary data
     *
     * \return get size of the binary data
     */
    std::size_t size() const noexcept;

    /*!
     * \brief get reference count
     *
     * \return reference count
     */
    std::size_t use_count() const noexcept;

    /*!
     * \brief clear data
     */
    void reset() noexcept;

private:
    //! struct for internal data
    struct impl;

    //! internal data
    impl* impl_{nullptr};
};

/*!
 * \brief compare data
 *
 * \param left left-hand-side data
 * \param right right-hand-side data
 * \return whether two data vectors are same
 */
MPRPC_EXPORT bool operator==(
    const shared_binary& left, const shared_binary& right) noexcept;

/*!
 * \brief compare data
 *
 * \param left left-hand-side data
 * \param right right-hand-side data
 * \return whether two data vectors are same
 */
MPRPC_EXPORT bool operator!=(
    const shared_binary& left, const shared_binary& right) noexcept;

}  // namespace mprpc

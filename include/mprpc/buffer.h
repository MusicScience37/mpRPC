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
 * \brief declaration and implementation of buffer class
 */
#pragma once

#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>

#include "mprpc/require_nonull.h"

namespace mprpc {

/*!
 * \brief class of buffers
 */
class buffer {
public:
    //! construct
    buffer() : buffer(0) {}

    /*!
     * \brief construct
     *
     * \param size size
     */
    explicit buffer(std::size_t size) {
        constexpr std::size_t initial_capacity = 1024;
        const std::size_t capacity =
            calculate_next_capacity(initial_capacity, size);
        data_ = allocate(capacity);
        size_ = size;
        capacity_ = capacity;
    }

    /*!
     * \brief construct
     *
     * \param data data to copy
     * \param size data size
     */
    buffer(const char* data, std::size_t size) : buffer(size) {
        MPRPC_REQUIRE_NONULL(data);
        std::memcpy(data_, data, size);
    }

    /*!
     * \brief get pointer
     *
     * \return pointer
     */
    char* data() { return data_; }

    /*!
     * \brief get pointer
     *
     * \return pointer
     */
    const char* data() const noexcept { return data_; }

    /*!
     * \brief get size
     *
     * \return size
     */
    std::size_t size() const noexcept { return size_; }

    /*!
     * \brief get maximum size usable
     *
     * \return maximum size
     */
    static std::size_t max_size() noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    /*!
     * \brief get capacity
     *
     * \return capacity
     */
    std::size_t capacity() const noexcept { return capacity_; }

    /*!
     * \brief change size
     *
     * \param size new size
     */
    void resize(std::size_t size) {
        if (size <= capacity_) {
            size_ = size;
            return;
        }
        const std::size_t capacity = calculate_next_capacity(capacity_, size);
        data_ = reallocate(data_, capacity);
        size_ = size;
        capacity_ = capacity;
    }

    /*!
     * \brief change capacity
     *
     * \param size new capacity
     */
    void reserve(std::size_t size) {
        if (size <= capacity_) {
            return;
        }
        const std::size_t capacity = calculate_next_capacity(capacity_, size);
        data_ = reallocate(data_, capacity);
        capacity_ = capacity;
    }

    /*!
     * \brief remove size bytes from beggining of the data
     *
     * \param size number of bytes removed
     */
    void consume(std::size_t size) noexcept {
        if (size >= size_) {
            size_ = 0;
            return;
        }
        const std::size_t moved_size = size_ - size;
        std::memmove(data_, data_ + size, moved_size);
        size_ = moved_size;
    }

    /*!
     * \brief destruct
     */
    ~buffer() { deallocate(data_); }

    buffer(const buffer&) = delete;
    buffer(buffer&&) = delete;
    buffer& operator=(const buffer&) = delete;
    buffer& operator=(buffer&&) = delete;

private:
    /*!
     * \brief calculate next capacity
     *
     * \param current_capacity current capacity
     * \param required_size required size
     * \return next capacity
     */
    static std::size_t calculate_next_capacity(
        std::size_t current_capacity, std::size_t required_size) {
        std::size_t capacity = current_capacity;
        constexpr std::size_t multiplier = 2;
        if (required_size > max_size() / multiplier) {
            return required_size;
        }
        while (capacity < required_size) {
            capacity *= multiplier;
        }
        return capacity;
    }

    /*!
     * \brief allocate memory
     *
     * \param size size
     * \return pointer
     */
    static char* allocate(std::size_t size) {
        // NOLINTNEXTLINE: this class is a container
        void* ptr = std::malloc(size);
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<char*>(ptr);
    }

    /*!
     * \brief reallocate memory
     *
     * \param ptr old pointer
     * \param size new size
     * \return pointer
     */
    static char* reallocate(char* ptr, std::size_t size) {
        // NOLINTNEXTLINE: this class is a container
        void* new_ptr = std::realloc(ptr, size);
        if (new_ptr == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<char*>(new_ptr);
    }

    /*!
     * \brief deallocate memory
     *
     * \param ptr pointer
     */
    static void deallocate(char* ptr) noexcept {
        // NOLINTNEXTLINE: this class is a container
        std::free(ptr);
    }

    //! data
    char* data_{nullptr};

    //! size
    std::size_t size_{0};

    //! capacity
    std::size_t capacity_{0};
};

}  // namespace mprpc

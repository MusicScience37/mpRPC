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
 * \brief declaration and implementation of pod_vector class
 */
#pragma once

#include <algorithm>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "stl_ext/allocator_traits.h"
#include "stl_ext/impl/safe_double_size.h"
#include "stl_ext/impl/vector_iterator.h"
#include "stl_ext/pod_allocator.h"

namespace stl_ext {

/*!
 * \brief class of vectors for POD
 *
 * \tparam ValueType value type
 * \tparam Allocator allocator
 */
template <typename ValueType, typename Allocator = pod_allocator<ValueType>>
class pod_vector {
public:
    static_assert(std::is_trivially_copyable<ValueType>::value,
        "ValueType must be trivially copyable");
    static_assert(alignof(ValueType) <= alignof(std::max_align_t),
        "ValueType with large alignment cannot be used");

    //! value type
    using value_type = ValueType;
    //! allocator type
    using allocator_type = Allocator;
    //! allocator traits type
    using allocator_traits_type = allocator_traits<allocator_type>;
    //! type of size and indices
    using size_type = typename allocator_traits_type::size_type;
    //! type of differences of pointers
    using difference_type = typename allocator_traits_type::difference_type;
    //! type of references
    using reference = value_type&;
    //! type of constant references
    using const_reference = const value_type&;
    //! type of pointers
    using pointer = typename allocator_traits_type::pointer;
    //! type of constant pointers
    using const_pointer = typename allocator_traits_type::const_pointer;
    //! type of iterators
    using iterator =
        impl::vector_iterator<pod_vector<value_type, allocator_type>, pointer,
            reference>;
    //! type of constant iterators
    using const_iterator =
        impl::vector_iterator<pod_vector<value_type, allocator_type>,
            const_pointer, const_reference>;
    //! type of reverse iterator
    using reverse_iterator = std::reverse_iterator<iterator>;
    //! type of constant reverse iterator
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //! construct empty vector
    pod_vector() : pod_vector(0) {}

    /*!
     * \brief construct with size
     *
     * \param size size of vector
     * \param alloc allocator
     */
    explicit pod_vector(
        size_type size, const allocator_type& alloc = allocator_type())
        : alloc_(alloc) {
        const size_type capacity = next_capacity(size);
        data_ = allocator_traits_type::allocate(alloc_, capacity);
        capacity_ = capacity;
        size_ = size;
    }

    /*!
     * \brief construct with data
     *
     * \param iter iterator to data
     * \param size size of data
     * \param alloc allocator
     */
    template <typename InputIterator>
    pod_vector(InputIterator iter, size_type size,
        const allocator_type& alloc = allocator_type())
        : pod_vector(size, alloc) {
        pointer data_ptr = data_;
        for (size_type i = 0; i < size; ++i) {
            *data_ptr = *iter;
            ++data_ptr;
            ++iter;
        }
    }

    /*!
     * \brief construct from initializer list
     *
     * \param data initializer list
     * \param alloc allocator
     */
    pod_vector(std::initializer_list<value_type> data,
        const allocator_type& alloc = allocator_type())
        : pod_vector(data.begin(), data.size(), alloc) {}

    /*!
     * \brief copy construct
     *
     * \param vec another vector
     */
    pod_vector(const pod_vector& vec)
        : pod_vector(vec.data_, vec.size_, vec.alloc_) {}

    /*!
     * \brief copy assign a vector
     *
     * \param vec another vector
     * \return this object
     */
    pod_vector& operator=(const pod_vector& vec) {
        if (this == &vec) {
            return *this;
        }
        resize(vec.size_);
        pointer input = vec.data_;
        pointer data_ptr = data_;
        for (size_type i = 0; i < vec.size_; ++i) {
            *data_ptr = *input;
            ++data_ptr;
            ++input;
        }
        return *this;
    }

    /*!
     * \brief move construct
     *
     * \param vec another vector
     */
    pod_vector(pod_vector&& vec) noexcept
        : data_(std::exchange(vec.data_, nullptr)),
          capacity_(std::exchange(vec.capacity_, 0)),
          size_(std::exchange(vec.size_, 0)) {}

    /*!
     * \brief move assign a vector
     *
     * \param vec another vector
     * \return this object
     */
    pod_vector& operator=(pod_vector&& vec) noexcept {
        swap(vec);
        return *this;
    }

    /*!
     * \brief destruct
     */
    ~pod_vector() {
        allocator_traits_type::deallocate(alloc_, data_, capacity_);
    }

    /*!
     * \brief swap with another vector
     *
     * \param vec another vector
     */
    void swap(pod_vector& vec) {
        std::swap(data_, vec.data_);
        std::swap(capacity_, vec.capacity_);
        std::swap(size_, vec.size_);
    }

    /*!
     * \brief change size
     *
     * \param size size
     */
    void resize(size_type size) {
        reserve(size);
        size_ = size;
    }

    /*!
     * \brief reserve capacity
     *
     * \param size capacity
     */
    void reserve(size_type size) {
        if (size <= capacity_) {
            return;
        }
        const size_type capacity = next_capacity(size);
        data_ = allocator_traits_type::reallocate(alloc_, capacity, data_);
        capacity_ = capacity;
    }

    /*!
     * \brief clear data
     */
    void clear() { resize(0); }

    /*!
     * \brief access to pos-th data with boundary check
     *
     * \param pos position
     * \return data
     */
    reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range(
                "stl_ext::pod_vector::at position out of range");
        }
        return data_[pos];
    }

    /*!
     * \brief access to pos-th data with boundary check
     *
     * \param pos position
     * \return data
     */
    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range(
                "stl_ext::pod_vector::at position out of range");
        }
        return data_[pos];
    }

    /*!
     * \brief access to pos-th data
     *
     * \param pos position
     * \return data
     */
    reference operator[](size_type pos) noexcept { return data_[pos]; }

    /*!
     * \brief access to pos-th data
     *
     * \param pos position
     * \return data
     */
    const_reference operator[](size_type pos) const noexcept {
        return data_[pos];
    }

    /*!
     * \brief get pointer to data
     *
     * \return pointer
     */
    pointer data() noexcept { return data_; }

    /*!
     * \brief get pointer to data
     *
     * \return pointer
     */
    const_pointer data() const noexcept { return data_; }

    /*!
     * \brief get size
     *
     * \return size
     */
    size_type size() const noexcept { return size_; }

    /*!
     * \brief get capacity
     *
     * \return capacity
     */
    size_type capacity() const noexcept { return capacity_; }

    /*!
     * \brief check whether this vector is empty
     *
     * \return whether this vector is empty
     */
    bool empty() const noexcept { return size_ == 0; }

    /*!
     * \brief get iterator to the beggining
     *
     * \return iterator
     */
    iterator begin() noexcept { return iterator(data_); }

    /*!
     * \brief get iterator to the beggining
     *
     * \return iterator
     */
    const_iterator begin() const noexcept { return const_iterator(data_); }

    /*!
     * \brief get iterator to the beggining
     *
     * \return iterator
     */
    const_iterator cbegin() const noexcept { return const_iterator(data_); }

    /*!
     * \brief get iterator to the end
     *
     * \return iterator
     */
    iterator end() noexcept { return iterator(data_ + size_); }

    /*!
     * \brief get iterator to the end
     *
     * \return iterator
     */
    const_iterator end() const noexcept {
        return const_iterator(data_ + size_);
    }

    /*!
     * \brief get iterator to the end
     *
     * \return iterator
     */
    const_iterator cend() const noexcept {
        return const_iterator(data_ + size_);
    }

    /*!
     * \brief insert elements
     *
     * \tparam InputIterator type of input iterator
     * \param pos iterator to the position to insert to
     * \param first iterator to the first element to insert
     * \param last iterator to the last element to insert
     * \return first element inserted
     */
    template <typename InputIterator>
    iterator insert(
        const_iterator pos, InputIterator first, InputIterator last) {
        const auto num_elements =
            static_cast<size_type>(std::distance(first, last));
        if (num_elements == 0) {
            return iterator(data_ + (pos.ptr() - data_));
        }

        const difference_type pos_ind = pos.ptr() - data_;
        const size_type current_size = size_;
        resize(current_size + num_elements);
        std::copy_backward(
            data_ + pos_ind, data_ + current_size, data_ + size_);
        std::copy(first, last, data_ + pos_ind);
        return iterator(data_ + pos_ind);
    }

    /*!
     * \brief insert a value
     *
     * \param pos position
     * \param value value
     * \return iterator to the added value
     */
    iterator insert(const_iterator pos, value_type value) {
        const difference_type pos_ind = pos.ptr() - data_;
        resize(size_ + 1);
        std::copy_backward(data_ + pos_ind, data_ + size_ - 1, data_ + size_);
        data_[pos_ind] = value;
        return iterator(data_ + pos_ind);
    }

    /*!
     * \brief insert a value to the end
     *
     * \param value value
     */
    void push_back(value_type value) {
        resize(size_ + 1);
        data_[size_ - 1] = value;
    }

    /*!
     * \brief erase elements in a range
     *
     * \param first iterator to the first element
     * \param last iterator to the last element
     * \return iterator to the next element
     */
    iterator erase(iterator first, iterator last) {
        if (first >= last) {
            return last;
        }
        std::memmove(
            first.ptr(), last.ptr(), sizeof(value_type) * (end() - last));
        size_ -= last - first;
        return first;
    }

    /*!
     * \brief erase elements in a range
     *
     * \param first iterator to the first element
     * \param last iterator to the last element
     * \return iterator to the next element
     */
    iterator erase(const_iterator first, const_iterator last) {
        return erase(iterator(data_ + (first.ptr() - data_)),
            iterator(data_ + (last.ptr() - data_)));
    }

    /*!
     * \brief erase an element
     *
     * \param pos iterator to the element
     * \return iterator to the next element
     */
    iterator erase(iterator pos) { return erase(pos, pos + 1); }

    /*!
     * \brief erase an element
     *
     * \param pos iterator to the element
     * \return iterator to the next element
     */
    iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

private:
    /*!
     * \brief calculate the next capacity
     *
     * \param required_capacity required capacity
     * \return next capacity
     */
    size_type next_capacity(size_type required_capacity) {
        size_type capacity = capacity_;

        constexpr size_type min_capacity = 8;
        if (capacity < min_capacity) {
            capacity = min_capacity;
        }

        while (capacity < required_capacity) {
            capacity = impl::safe_double_size(capacity);
        }

        return capacity;
    }

    //! data
    pointer data_{nullptr};

    //! capacity
    size_type capacity_{0};

    //! size
    size_type size_{0};

    //! allocator
    allocator_type alloc_{};
};

/*!
 * \brief compare two vectors
 *
 * \tparam Value value type
 * \tparam Allocator allocator type
 * \param left left-hand-side object
 * \param right right-hand-side object
 * \return whether two vectors are same
 */
template <typename Value, typename Allocator>
bool operator==(const pod_vector<Value, Allocator>& left,
    const pod_vector<Value, Allocator>& right) {
    return std::equal(left.begin(), left.end(), right.begin(), right.end());
}

/*!
 * \brief compare two vectors
 *
 * \tparam Value value type
 * \tparam Allocator allocator type
 * \param left left-hand-side object
 * \param right right-hand-side object
 * \return whether two vectors are different
 */
template <typename Value, typename Allocator>
bool operator!=(const pod_vector<Value, Allocator>& left,
    const pod_vector<Value, Allocator>& right) {
    return !operator==(left, right);
}

}  // namespace stl_ext

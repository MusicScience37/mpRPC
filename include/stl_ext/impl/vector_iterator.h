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
 * \brief declaration and implementation of vector_iterator class
 */
#pragma once

#include <iterator>
#include <type_traits>

namespace stl_ext {
namespace impl {

/*!
 * \brief class of iterators of vectors
 *
 * \tparam VectorType type of vectors
 * \tparam PointerType type of pointers
 * \tparam ReferenceType type of references
 */
template <typename VectorType, typename PointerType, typename ReferenceType>
class vector_iterator {
public:
    //! type of vectors
    using vector_type = VectorType;
    //! value type
    using value_type = typename vector_type::value_type;
    //! type of differences of pointers
    using difference_type = typename vector_type::difference_type;
    //! type of references
    using reference = ReferenceType;
    //! type of pointers
    using pointer = PointerType;
    //! type of iterator category
    using iterator_category = std::random_access_iterator_tag;

    //! construct (null iterator)
    vector_iterator() noexcept = default;

    /*!
     * \brief construct
     *
     * \param ptr pointer
     */
    explicit vector_iterator(pointer ptr) noexcept : ptr_(ptr) {}

    /*!
     * \brief copy
     */
    vector_iterator(const vector_iterator&) noexcept = default;

    /*!
     * \brief copy
     *
     * \return this object
     */
    vector_iterator& operator=(const vector_iterator&) noexcept = default;

    /*!
     * \brief move
     */
    vector_iterator(vector_iterator&&) noexcept = default;

    /*!
     * \brief move
     *
     * \return this object
     */
    vector_iterator& operator=(vector_iterator&&) noexcept = default;

    /*!
     * \brief destruct
     */
    ~vector_iterator() = default;

    /*!
     * \brief construct from other iterator
     *
     * \tparam Vector type of vector
     * \tparam Pointer type of pointer
     * \tparam Reference type of reference
     * \param obj another iterator object
     */
    template <typename Vector, typename Pointer, typename Reference,
        typename =
            std::enable_if_t<std::is_convertible<Pointer, pointer>::value>>
    // NOLINTNEXTLINE: implicit convertion is intended
    vector_iterator(const vector_iterator<Vector, Pointer, Reference>& obj)
        : ptr_(obj.ptr()) {}

    /*!
     * \brief dereference this iterator
     *
     * \return reference
     */
    reference operator*() const noexcept { return *ptr_; }

    /*!
     * \brief access to member of value
     *
     * \return pointer
     */
    pointer operator->() const noexcept { return ptr_; }

    /*!
     * \brief access to value with offset
     *
     * \param n offset
     * \return value
     */
    reference operator[](difference_type n) const noexcept {
        return *(ptr_ + n);
    }

    /*!
     * \brief increment this iterator
     *
     * \return this object
     */
    vector_iterator& operator++() noexcept {
        ++ptr_;
        return *this;
    }

    /*!
     * \brief decrement this iterator
     *
     * \return this object
     */
    vector_iterator& operator--() noexcept {
        --ptr_;
        return *this;
    }

    /*!
     * \brief increment this iterator
     *
     * \return this object
     */
    const vector_iterator operator++(  // NOLINT: conflict of warnings
        int) noexcept {
        vector_iterator copy(*this);
        operator++();
        return copy;
    }

    /*!
     * \brief decrement this iterator
     *
     * \return this object
     */
    const vector_iterator operator--(  // NOLINT: conflict of warnings
        int) noexcept {
        vector_iterator copy(*this);
        operator--();
        return copy;
    }

    /*!
     * \brief add integer to this iterator
     *
     * \param n offset
     * \return this object
     */
    vector_iterator& operator+=(difference_type n) noexcept {
        ptr_ += n;
        return *this;
    }

    /*!
     * \brief subtract integer from this iterator
     *
     * \param n offset
     * \return this object
     */
    vector_iterator& operator-=(difference_type n) noexcept {
        ptr_ -= n;
        return *this;
    }

    /*!
     * \brief subtract another iterator
     *
     * \param right another iterator
     * \return difference
     */
    difference_type operator-(const vector_iterator& right) const noexcept {
        return ptr_ - right.ptr();
    }

    /*!
     * \brief compare with another iterator
     *
     * \tparam Vector type of vector
     * \tparam Pointer type of pointer
     * \tparam Reference type of reference
     * \param right another iterator
     * \return result of this == right
     */
    template <typename Vector, typename Pointer, typename Reference>
    bool operator==(const vector_iterator<Vector, Pointer, Reference>& right)
        const noexcept {
        return ptr_ == right.ptr();
    }

    /*!
     * \brief compare with another iterator
     *
     * \tparam Vector type of vector
     * \tparam Pointer type of pointer
     * \tparam Reference type of reference
     * \param right another iterator
     * \return result of this != right
     */
    template <typename Vector, typename Pointer, typename Reference>
    bool operator!=(const vector_iterator<Vector, Pointer, Reference>& right)
        const noexcept {
        return ptr_ != right.ptr();
    }

    /*!
     * \brief compare with another iterator
     *
     * \tparam Vector type of vector
     * \tparam Pointer type of pointer
     * \tparam Reference type of reference
     * \param right another iterator
     * \return result of this < right
     */
    template <typename Vector, typename Pointer, typename Reference>
    bool operator<(const vector_iterator<Vector, Pointer, Reference>& right)
        const noexcept {
        return ptr_ < right.ptr();
    }

    /*!
     * \brief compare with another iterator
     *
     * \tparam Vector type of vector
     * \tparam Pointer type of pointer
     * \tparam Reference type of reference
     * \param right another iterator
     * \return result of this <= right
     */
    template <typename Vector, typename Pointer, typename Reference>
    bool operator<=(const vector_iterator<Vector, Pointer, Reference>& right)
        const noexcept {
        return ptr_ <= right.ptr();
    }

    /*!
     * \brief compare with another iterator
     *
     * \tparam Vector type of vector
     * \tparam Pointer type of pointer
     * \tparam Reference type of reference
     * \param right another iterator
     * \return result of this > right
     */
    template <typename Vector, typename Pointer, typename Reference>
    bool operator>(const vector_iterator<Vector, Pointer, Reference>& right)
        const noexcept {
        return ptr_ > right.ptr();
    }

    /*!
     * \brief compare with another iterator
     *
     * \tparam Vector type of vector
     * \tparam Pointer type of pointer
     * \tparam Reference type of reference
     * \param right another iterator
     * \return result of this >= right
     */
    template <typename Vector, typename Pointer, typename Reference>
    bool operator>=(const vector_iterator<Vector, Pointer, Reference>& right)
        const noexcept {
        return ptr_ >= right.ptr();
    }

    /*!
     * \brief get pointer
     *
     * \return pointer
     */
    pointer ptr() const noexcept { return ptr_; }

private:
    //! pointer to data
    pointer ptr_{nullptr};
};

/*!
 * \brief add offset to iterator
 *
 * \tparam VectorType type of vectors
 * \tparam PointerType type of pointers
 * \tparam ReferenceType type of references
 * \param iter iterator
 * \param n offset
 * \return added iterator
 */
template <typename VectorType, typename PointerType, typename ReferenceType>
inline vector_iterator<VectorType, PointerType, ReferenceType> operator+(
    const vector_iterator<VectorType, PointerType, ReferenceType>& iter,
    typename vector_iterator<VectorType, PointerType,
        ReferenceType>::difference_type n) noexcept {
    return vector_iterator<VectorType, PointerType, ReferenceType>(iter) += n;
}

/*!
 * \brief add offset to iterator
 *
 * \tparam VectorType type of vectors
 * \tparam PointerType type of pointers
 * \tparam ReferenceType type of references
 * \param n offset
 * \param iter iterator
 * \return added iterator
 */
template <typename VectorType, typename PointerType, typename ReferenceType>
inline vector_iterator<VectorType, PointerType, ReferenceType> operator+(
    typename vector_iterator<VectorType, PointerType,
        ReferenceType>::difference_type n,
    const vector_iterator<VectorType, PointerType, ReferenceType>&
        iter) noexcept {
    return vector_iterator<VectorType, PointerType, ReferenceType>(iter) += n;
}

/*!
 * \brief subtract offset from iterator
 *
 * \tparam VectorType type of vectors
 * \tparam PointerType type of pointers
 * \tparam ReferenceType type of references
 * \param iter iterator
 * \param n offset
 * \return subtracted iterator
 */
template <typename VectorType, typename PointerType, typename ReferenceType>
inline vector_iterator<VectorType, PointerType, ReferenceType> operator-(
    const vector_iterator<VectorType, PointerType, ReferenceType>& iter,
    typename vector_iterator<VectorType, PointerType,
        ReferenceType>::difference_type n) noexcept {
    return vector_iterator<VectorType, PointerType, ReferenceType>(iter) -= n;
}

}  // namespace impl
}  // namespace stl_ext

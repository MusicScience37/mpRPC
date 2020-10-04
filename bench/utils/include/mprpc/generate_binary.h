#pragma once

#include <random>
#include <string>

namespace mprpc {

/*!
 * \brief generate random binary data
 *
 * \param size size of binary data
 * \return string
 */
inline std::string generate_binary(std::size_t size) {
    // NOLINTNEXTLINE: for reproducibility of benchmarks
    std::mt19937 engine;
    constexpr int min_char = 0x00;
    constexpr int max_char = 0xFF;
    std::uniform_int_distribution<int> dist(min_char, max_char);
    std::string data;
    data.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        data.push_back(static_cast<char>(dist(engine)));
    }
    return data;
}

}  // namespace mprpc

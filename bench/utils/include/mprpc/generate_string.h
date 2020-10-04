#pragma once

#include <random>
#include <string>

namespace mprpc {

/*!
 * \brief generate random string
 *
 * \param size size of string
 * \return string
 */
inline std::string generate_string(std::size_t size) {
    // NOLINTNEXTLINE: for reproducibility of benchmarks
    std::mt19937 engine;
    constexpr int min_char = 0x20;
    constexpr int max_char = 0x7E;
    std::uniform_int_distribution<int> dist(min_char, max_char);
    std::string data;
    data.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        data.push_back(static_cast<char>(dist(engine)));
    }
    return data;
}

}  // namespace mprpc

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
 * \brief definition of logging macros
 */
#pragma once

#include "logger.h"

/*!
 * \brief macro to get file name
 */
#define MPRPC_FILENAME __FILE__  // NOLINT: use of macro is necessary

/*!
 * \brief macro to get line number
 */
#define MPRPC_LINE __LINE__  // NOLINT: use of macro is necessary

#ifdef MPRPC_DOCUMENTATION
/*!
 * \brief macro to get function name
 *
 * This macro will be expanded to implementation-defined macros or variables.
 */
#define MPRPC_FUNCTION <implementation defined strings>
#elif __GNUC__                              // GCC and Clang
#define MPRPC_FUNCTION __PRETTY_FUNCTION__  // NOLINT: use of macro is necessary
#elif _MSC_VER                              // MSVC
#define MPRPC_FUNCTION __FUNCSIG__          // NOLINT: use of macro is necessary
#else                                       // fallback (C++ standard)
#define MPRPC_FUNCTION __func__             // NOLINT: use of macro is necessary
#endif

/*!
 * \brief macro to write log
 *
 * \param LOGGER_PTR pointer to logger
 * \param LOG_LEVEL log level
 *
 * Remaining arguments will be treated as format and data.
 */
#define MPRPC_LOG(LOGGER_PTR, LOG_LEVEL, ...) /*NOLINT*/                 \
    (LOGGER_PTR)                                                         \
        ->write(MPRPC_FILENAME, MPRPC_LINE, MPRPC_FUNCTION, (LOG_LEVEL), \
            __VA_ARGS__)

/*!
 * \brief macro to write trace log
 *
 * \param LOGGER_PTR pointer to logger
 *
 * Remaining arguments will be treated as format and data.
 */
#define MPRPC_TRACE(LOGGER_PTR, ...) /*NOLINT*/ \
    MPRPC_LOG((LOGGER_PTR), ::mprpc::logging::log_level::trace, __VA_ARGS__)

/*!
 * \brief macro to write debug log
 *
 * \param LOGGER_PTR pointer to logger
 *
 * Remaining arguments will be treated as format and data.
 */
#define MPRPC_DEBUG(LOGGER_PTR, ...) /*NOLINT*/ \
    MPRPC_LOG((LOGGER_PTR), ::mprpc::logging::log_level::debug, __VA_ARGS__)

/*!
 * \brief macro to write info log
 *
 * \param LOGGER_PTR pointer to logger
 *
 * Remaining arguments will be treated as format and data.
 */
#define MPRPC_INFO(LOGGER_PTR, ...) /*NOLINT*/ \
    MPRPC_LOG((LOGGER_PTR), ::mprpc::logging::log_level::info, __VA_ARGS__)

/*!
 * \brief macro to write warn log
 *
 * \param LOGGER_PTR pointer to logger
 *
 * Remaining arguments will be treated as format and data.
 */
#define MPRPC_WARN(LOGGER_PTR, ...) /*NOLINT*/ \
    MPRPC_LOG((LOGGER_PTR), ::mprpc::logging::log_level::warn, __VA_ARGS__)

/*!
 * \brief macro to write error log
 *
 * \param LOGGER_PTR pointer to logger
 *
 * Remaining arguments will be treated as format and data.
 */
#define MPRPC_ERROR(LOGGER_PTR, ...) /*NOLINT*/ \
    MPRPC_LOG((LOGGER_PTR), ::mprpc::logging::log_level::error, __VA_ARGS__)

/*!
 * \brief macro to write fatal log
 *
 * \param LOGGER_PTR pointer to logger
 *
 * Remaining arguments will be treated as format and data.
 */
#define MPRPC_FATAL(LOGGER_PTR, ...) /*NOLINT*/ \
    MPRPC_LOG((LOGGER_PTR), ::mprpc::logging::log_level::fatal, __VA_ARGS__)

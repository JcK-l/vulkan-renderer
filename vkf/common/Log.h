////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Log.h
/// \brief This file defines macros for logging messages at various levels of severity.
///
/// The macros in this file use the spdlog library to log messages. The available macros are LOG_INFO, LOG_WARN,
/// LOG_ERROR, and LOG_DEBUG. Each macro logs a message at a different level of severity. LOG_ERROR also includes the
/// filename and line number where the error occurred.
///
/// \author Joshua Lowe
/// \date 11/6/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef ROOT_PATH_SIZE
///
/// \def ROOT_PATH_SIZE
/// \brief A macro that defines the size of the root path. This is used to trim the root path from the filename in log
/// messages.
///
#define ROOT_PATH_SIZE 0
#endif

///
/// \def __FILENAME__
/// \brief A macro that trims the root path from the filename in log messages.
///
#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

#define LOG_INFO(...) spdlog::info(__VA_ARGS__);
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__);
#define LOG_ERROR(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__);
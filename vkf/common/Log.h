/// \file
/// \brief

//
// Created by Joshua Lowe on 11/6/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_LOG_H
#define VULKANRENDERER_LOG_H

// Mainly for IDEs
#ifndef ROOT_PATH_SIZE
#	define ROOT_PATH_SIZE 0
#endif

#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

#include <spdlog/spdlog.h>

#if !defined( NDEBUG )
#define LOG_INFO(...) spdlog::info(__VA_ARGS__);
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__);
#define LOG_ERROR(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__);
#else
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_DEBUG(...)
#endif

#endif //VULKANRENDERER_LOG_H

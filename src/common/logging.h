/// \file
/// \brief

//
// Created by Joshua Lowe on 11/6/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_LOGGING_H
#define VULKANRENDERER_LOGGING_H

#include <spdlog/spdlog.h>

// Mainly for IDEs
#ifndef ROOT_PATH_SIZE
#	define ROOT_PATH_SIZE 0
#endif

#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

#define LOGI(...) spdlog::info(__VA_ARGS__);
#define LOGW(...) spdlog::warn(__VA_ARGS__);
#define LOGE(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOGD(...) spdlog::debug(__VA_ARGS__);

/// \brief Collection of logging functions
namespace vkf::logging {
    /// \brief Initiates the spdlog logger and sets it to default
    void init();
} // namespace vkf

#endif //VULKANRENDERER_LOGGING_H

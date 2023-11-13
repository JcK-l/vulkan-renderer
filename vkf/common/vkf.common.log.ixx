/// \file
/// \brief

//
// Created by Joshua Lowe on 11/13/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>

export module vkf.common.log;

/// \brief Collection of logging functions
export namespace vkf::common::log {
    /// \brief Initiates the spdlog logger and sets it to default
    void init();
} // namespace vkf::common::log

//@formatter:off
module : private;
//@formatter:on

namespace vkf::common::log {
    void init() {
      try {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
        auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
#if !defined( NDEBUG )
        logger->
          set_level(spdlog::level::debug);
#else
        logger->set_level(spdlog::level::info);
#endif
        spdlog::set_pattern("[%^%l%$] %v");
        spdlog::set_default_logger(logger);
        logger->info("Logger initialized");
      } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
      }
    }
} // namespace vkf::common::log

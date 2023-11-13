/// \file
/// \brief

//
// Created by Joshua Lowe on 11/12/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include <memory>
#include "../common/Event.h"
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module vkf.platform.Application;

import vkf.core.Instance;
import vkf.core.Device;
import vkf.platform.Window;

/// \brief Vulkan framework
export namespace vkf::platform {

    class Application {
    public:
        Application() = default;

        explicit Application(const std::string &appName);

        ~Application() = default;

        Application(const Application &) = delete;

        Application(Application &&) = delete;

        Application &operator=(const Application &) = delete;

        Application &operator=(Application &&) = delete;


        /// \brief Main loop
        void run();

    private:
        void onUpdate();

        void onEvent(common::Event &event);

        std::unique_ptr<Window> window;
        std::unique_ptr<core::Instance> instance;
        std::unique_ptr<vk::raii::SurfaceKHR> surface;
        std::unique_ptr<core::Device> device;

        std::vector<const char *> instanceExtensions;
        std::vector<const char *> deviceExtensions;
    };
} // namespace vkf::platform

//@formatter:off
//module : private;
//@formatter:on


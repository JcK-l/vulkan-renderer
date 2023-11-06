/// \file
/// \brief

//
// Created by Joshua Lowe on 10/30/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_APPLICATION_H
#define VULKANRENDERER_APPLICATION_H

#include "Window.h"
#include "../core/Instance.h"

/// \brief Vulkan framework
namespace vkf {
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
        void initVulkan();

        void onUpdate();

        void onEvent(Event &event);

        std::unique_ptr<Window> window;
        core::Instance instance{""};
    };
} // namespace vkf


#endif //VULKANRENDERER_APPLICATION_H

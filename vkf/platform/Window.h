/// \file
/// \brief

//
// Created by Joshua Lowe on 10/31/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_WINDOW_H
#define VULKANRENDERER_WINDOW_H

#include "GLFW/glfw3.h"
#include "../common/event.h"
#include "../core/Instance.h"

/// \brief Collection of platform specific functionality
namespace vkf::platform {

    class Application;

    /// \brief This class provides functionality associated with glfw
    class Window {

    public:
        /// \brief Extent of the window
        struct Extent {
            uint32_t width;
            uint32_t height;
        };

        /// \brief Vsync option
        enum class Vsync {
            OFF,
            ON
        };

        /// \brief Window mode
        enum class Mode {
            Fullscreen,
            FullscreenBorderless,
            Windowed
        };

        /// \brief Window properties
        struct Properties {
            std::string title;
            Mode mode{Mode::Windowed};
            bool resizeable{false};
            bool resized{false};
            Vsync vsync{Vsync::ON};
            Extent extent{800, 600};
            std::function<void(Event &)> eventCallback;
        };

        explicit Window(Properties);

        ~Window();

        void onUpdate();

        void setEventCallback(const std::function<void(Event &)> &callback);

        [[nodiscard]] bool isClosed() const;

        [[nodiscard]] GLFWwindow *getHandle() const;

        std::vector<const char *> getRequiredSurfaceExtensions();

        vk::raii::SurfaceKHR createSurface(const core::Instance &instance);

    private:
        Properties windowData;

        GLFWwindow *handle{nullptr};
    };
} // namespace vkf::platform



#endif //VULKANRENDERER_WINDOW_H

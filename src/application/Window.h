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

namespace vkf {

    class Application;

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

        inline void
        setEventCallback(const std::function<void(Event &)> &callback) { windowData.eventCallback = callback; };

        [[nodiscard]] inline bool isClosed() const { return glfwWindowShouldClose(handle); };

        [[nodiscard]] inline GLFWwindow *getHandle() const { return handle; };

    private:
        Properties windowData;

        GLFWwindow *handle{nullptr};
    };
} // namespace vkf


#endif //VULKANRENDERER_WINDOW_H

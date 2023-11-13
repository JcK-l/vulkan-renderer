/// \file
/// \brief

//
// Created by Joshua Lowe on 11/12/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include "GLFW/glfw3.h"
#include "../common/Event.h"

#include <string>
#include <functional>
#include <vulkan/vulkan_raii.hpp>

export module vkf.platform.Window;

import vkf.core.Instance;

export namespace vkf::platform {

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
            std::function<void(common::Event &)> eventCallback;
        };

        explicit Window(Properties);

        ~Window();

        void onUpdate();

        void setEventCallback(const std::function<void(common::Event &)> &callback);

        [[nodiscard]] bool isClosed() const;

        [[nodiscard]] GLFWwindow *getHandle() const;

        static std::vector<const char *> getRequiredSurfaceExtensions();

        vk::raii::SurfaceKHR createSurface(const core::Instance &instance);

    private:
        Properties windowData;

        GLFWwindow *handle{nullptr};
    };
} // namespace vkf::platform

//@formatter:off
//module : private;
//@formatter:on



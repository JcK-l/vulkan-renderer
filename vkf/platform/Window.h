////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Window.h
/// \brief This file declares the Window class which is used for managing GLFW windows.
///
/// The Window class is part of the vkf::platform namespace. It provides an interface for interacting with a GLFW
/// window, including creating a surface, getting required surface extensions, setting event callbacks, and updating the
/// window. It also provides methods for querying window properties such as whether the window is closed, its
/// framebuffer size, and its handle.
///
/// \author Joshua Lowe
/// \date 10/31/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_WINDOW_H
#define VULKANRENDERER_WINDOW_H

#include "../common/Event.h"
#include "GLFW/glfw3.h"

namespace vkf::core // Forward declarations
{
class Instance;
} // namespace vkf::core

namespace vkf::platform
{

///
/// \class Window
/// \brief This class manages GLFW windows.
///
/// It provides an interface for interacting with a GLFW window, including creating a surface, getting required surface
/// extensions, setting event callbacks, and updating the window. It also provides methods for querying window
/// properties such as whether the window is closed, its framebuffer size, and its handle.
///
class Window
{

  public:
    ///
    /// \struct Extent
    /// \brief This struct represents the extent of the window.
    ///
    struct Extent
    {
        uint32_t width;
        uint32_t height;
    };

    ///
    /// \enum Vsync
    /// \brief This enum represents the Vsync option.
    ///
    enum class Vsync
    {
        OFF,
        ON
    };

    ///
    /// \enum Mode
    /// \brief This enum represents the window mode.
    ///
    enum class Mode
    {
        Fullscreen,
        FullscreenBorderless,
        Windowed
    };

    ///
    /// \struct Properties
    /// \brief This struct represents the properties of the window.
    ///
    struct Properties
    {
        std::string title;
        Mode mode{Mode::Windowed};
        bool resizeable{false};
        bool resized{false};
        Vsync vsync{Vsync::ON};
        Extent extent{800, 600};
        std::function<void(Event &)> eventCallback;
    };

    ///
    /// \brief Constructs a Window object.
    ///
    /// This constructor creates a GLFW window using the provided properties.
    ///
    /// \param properties The properties of the window.
    ///
    explicit Window(Properties properties);

    Window(const Window &) = delete;            // Deleted copy constructor
    Window(Window &&) noexcept = default;       // Default move constructor
    Window &operator=(const Window &) = delete; // Deleted copy assignment operator
    Window &operator=(Window &&) = delete;      // Deleted move assignment operator
    ~Window();                                  // Implementation in Window.cpp

    void onUpdate();

    void setEventCallback(const std::function<void(Event &)> &callback);

    [[nodiscard]] bool isClosed() const;

    [[nodiscard]] GLFWwindow *getHandle() const;

    std::vector<const char *> getRequiredSurfaceExtensions();

    [[nodiscard]] std::pair<uint32_t, uint32_t> getFramebufferSize() const;

    vk::raii::SurfaceKHR createSurface(const core::Instance &instance);

    void waitEvents() const;

    [[nodiscard]] bool isResized() const;
    void setResized(bool isResized);

  private:
    Properties windowData;

    GLFWwindow *handle{nullptr};
};
} // namespace vkf::platform

#endif // VULKANRENDERER_WINDOW_H
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Application.h
/// \brief This file declares the Application class which is used for managing the main application.
///
/// The Application class is part of the vkf::platform namespace. It provides an interface for interacting with the main
/// application, including running the main loop, initializing the logger, and creating the window, instance, surface,
/// and device. It also provides methods for enabling instance extensions, instance layers, and device extensions.
///
/// \author Joshua Lowe
/// \date 10/30/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_APPLICATION_H
#define VULKANRENDERER_APPLICATION_H

namespace vkf // Forward declarations
{
struct Event;
} // namespace vkf

namespace vkf::core // Forward declarations
{
class Instance;
class Device;
class Swapchain;
} // namespace vkf::core

namespace vkf::rendering // Forward declarations
{
class RenderManager;
} // namespace vkf::rendering

namespace vkf::platform
{

// Forward declarations
class Window;

///
/// \class Application
/// \brief This class manages the main application.
///
/// It provides an interface for interacting with the main application, including running the main loop, initializing
/// the logger, and creating the window, instance, surface, and device. It also provides methods for enabling instance
/// extensions, instance layers, and device extensions.
///
class Application
{
  public:
    ///
    /// \brief Constructs an Application object with a specified application name.
    ///
    /// \param appName The name of the application.
    ///
    explicit Application(std::string appName);

    Application(const Application &) = delete;            // Deleted copy constructor
    Application(Application &&) noexcept = default;       // Default move constructor
    Application &operator=(const Application &) = delete; // Deleted copy assignment operator
    Application &operator=(Application &&) = delete;      // Deleted move assignment operator
    ~Application(); // Implementation in Application.cpp because of std::unique_ptr forward declaration

    ///
    /// \brief Runs the main loop of the application.
    ///
    void run();

    ///
    /// \brief Initializes the logger for the application.
    ///
    static void initLogger();

  private:
    void onUpdate();

    void onEvent(Event &event);

    void createWindow();
    void createInstance();
    void createSurface();
    void createDevice();
    void createRenderManager();

    void enableInstanceExtension(const char *extensionName);
    void enableInstanceLayer(const char *layerName);
    void enableDeviceExtension(const char *extensionName);

    std::unique_ptr<Window> window;
    std::unique_ptr<core::Instance> instance;
    std::unique_ptr<vk::raii::SurfaceKHR> surface;
    std::unique_ptr<core::Device> device;
    std::unique_ptr<rendering::RenderManager> renderManager;

    std::vector<const char *> instanceExtensions;
    std::vector<const char *> instanceLayers;

    std::vector<const char *> deviceExtensions;
    const std::string appName;
};
} // namespace vkf::platform

#endif // VULKANRENDERER_APPLICATION_H
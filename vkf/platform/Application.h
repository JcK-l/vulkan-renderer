/// \file
/// \brief

//
// Created by Joshua Lowe on 10/30/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_APPLICATION_H
#define VULKANRENDERER_APPLICATION_H

#include "../core/Device.h"
#include "../core/Instance.h"
#include "../core/Swapchain.h"
#include "Window.h"

namespace vkf::platform
{

/// \brief Main application class
class Application
{
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

    static void initLogger();

  private:
    void onUpdate();

    void onEvent(Event &event);

    void createWindow();
    void createInstance();
    void createSurface();
    void createDevice();

    void enableInstanceExtension(const char *extensionName);
    void enableInstanceLayer(const char *layerName);
    void enableDeviceExtension(const char *extensionName);

    std::unique_ptr<Window> window;
    std::unique_ptr<core::Instance> instance;
    std::unique_ptr<vk::raii::SurfaceKHR> surface;
    std::unique_ptr<core::Device> device;
    std::unique_ptr<core::Swapchain> swapchain;

    std::vector<const char *> instanceExtensions;
    std::vector<const char *> instanceLayers;

    std::vector<const char *> deviceExtensions;
    const std::string appName;
};
} // namespace vkf::platform

#endif // VULKANRENDERER_APPLICATION_H

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

#pragma once

// Forward declarations
#include "../common/CommonFwd.h"
#include "../core/CoreFwd.h"
#include "../rendering/RenderingFwd.h"
#include "../scene/SceneFwd.h"
#include "PlatformFwd.h"

namespace vkf::platform
{

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

    Application(const Application &) = delete;            ///< Deleted copy constructor
    Application(Application &&) noexcept = default;       ///< Default move constructor
    Application &operator=(const Application &) = delete; ///< Deleted copy assignment operator
    Application &operator=(Application &&) = delete;      ///< Deleted move assignment operator
    ~Application(); ///< Implementation in Application.cpp because of std::unique_ptr forward declaration

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
    void createScene(const core::RenderPass &renderPass);
    void createRenderManager();
    void createBindlessManager();

    void enableInstanceExtension(const char *extensionName);
    void enableInstanceLayer(const char *layerName);
    void enableDeviceExtension(const char *extensionName);

    std::unique_ptr<Window> window;
    std::unique_ptr<core::Instance> instance;
    std::unique_ptr<vk::raii::SurfaceKHR> surface;
    std::unique_ptr<core::Device> device;
    std::unique_ptr<scene::Scene> scene;
    std::unique_ptr<rendering::BindlessManager> bindlessManager;
    std::unique_ptr<rendering::RenderManager> renderManager;

    std::shared_ptr<Gui> gui;
    std::shared_ptr<core::Swapchain> swapchain;

    std::vector<const char *> instanceExtensions;
    std::vector<const char *> instanceLayers;

    std::vector<const char *> deviceExtensions;
    const std::string appName;
};
} // namespace vkf::platform
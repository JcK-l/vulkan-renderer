////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Application.cpp
/// \brief This file implements the Application class which is used for managing the main application.
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

#include "Application.h"

#include "../common/Log.h"
#include "../common/Utility.h"
#include "../core/Device.h"
#include "../core/Framebuffer.h"
#include "../core/Instance.h"
#include "../core/PhysicalDevice.h"
#include "../core/RenderPass.h"
#include "../core/Swapchain.h"
#include "../rendering/FrameData.h"
#include "../rendering/RenderManager.h"
#include "../rendering/RenderSubstage.h"
#include "../rendering/Renderer.h"
#include "Window.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <utility>

namespace vkf::platform
{

Application::Application(std::string appName) : appName{std::move(appName)}
{
    try
    {
        createWindow();
        createInstance();
        createSurface();
        createDevice();
        createRenderManager();
    }
    catch (vk::SystemError &err)
    {
        LOG_ERROR("vk::SystemError: {}", err.what())
        exit(-1);
    }
    catch (std::exception &err)
    {
        LOG_ERROR("std::exception: {}", err.what())
        exit(-1);
    }
    catch (...)
    {
        LOG_ERROR("unknown error")
        exit(-1);
    }
}

Application::~Application() = default;

void Application::run()
{
    while (!window->isClosed())
    {
        onUpdate();
        window->onUpdate();
        //    drawFrame();
    }
    device->getHandle().waitIdle();
}

void Application::onEvent(Event &event)
{
    switch (event.type)
    {
    case Event::Type::Keyboard: {
        auto data = std::get<Event::Keyboard>(event.data);
        //        LOG_DEBUG("KeyboardEvent: keycode={}, action={}", data.keycode, data.action)
        break;
    }
    case Event::Type::MouseMove: {
        auto data = std::get<Event::MouseMove>(event.data);
        //        LOG_DEBUG("MouseMoveEvent: x={}, y={}", data.xPos, data.yPos)
        break;
    }
    case Event::Type::MouseButton: {
        auto data = std::get<Event::MouseButton>(event.data);
        //        LOG_DEBUG("MouseButtonEvent: button={}, action={}", data.button, data.action)
        break;
    }
    case Event::Type::MouseScroll: {
        auto data = std::get<Event::MouseScroll>(event.data);
        //        LOG_DEBUG("MouseScrollEvent: x={}, y={}", data.xScroll, data.yScroll)
        break;
    }
    case Event::Type::Resize: {
        auto data = std::get<Event::Resize>(event.data);
        //        LOG_DEBUG("ResizeEvent: width={}, height={}", data.newWidth, data.newHeight)
        break;
    }
    case Event::Type::Close: {
        //        LOG_DEBUG("CloseEvent")
        break;
    }
    }
}

void Application::onUpdate()
{
    renderManager->beginFrame();
    renderManager->beginRenderPass();

    renderManager->draw();

    renderManager->endRenderPass();
    renderManager->endFrame();
}

void Application::initLogger()
{
    try
    {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
        auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
#if !defined(NDEBUG)
        logger->set_level(spdlog::level::debug);
#else
        logger->set_level(spdlog::level::info);
#endif
        spdlog::set_pattern("[%^%l%$] %v");
        spdlog::set_default_logger(logger);
        logger->info("Initialized logger");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Application::createWindow()
{
    window = std::make_unique<Window>(Window::Properties{
        .title{"VulkanRenderer"}, .mode = Window::Mode::Windowed, .resizeable = true, .extent{1000, 1000}});

    window->setEventCallback([this](auto &&arg) { onEvent(std::forward<decltype(arg)>(arg)); });
}

void Application::createInstance()
{
    for (const char *extensionName : window->getRequiredSurfaceExtensions())
    {
        enableInstanceExtension(extensionName);
    }
    enableInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

#if !defined(NDEBUG)
    enableInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    enableInstanceLayer("VK_LAYER_KHRONOS_validation");
    enableInstanceLayer("VK_LAYER_LUNARG_monitor");
#endif

    instance = std::make_unique<core::Instance>(appName, instanceExtensions, instanceLayers);
}

void Application::createSurface()
{
    surface = std::make_unique<vk::raii::SurfaceKHR>(window->createSurface(*instance));

    if (!surface)
    {
        throw std::runtime_error("Failed to create window surface.");
    }
}

void Application::createDevice()
{
    enableDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    device = std::make_unique<core::Device>(*instance, *surface, deviceExtensions);
}

void Application::createRenderManager()
{

    auto swapchain = std::make_unique<core::Swapchain>(*device, *surface, *window);

    rendering::RenderOptions renderOptions{.clearValue = {std::array<float, 4>{0.53f, 0.81f, 0.92f, 1.0f}},
                                           .numSubpasses = 1};

    auto renderer = std::make_unique<rendering::Renderer>(*device, swapchain.get(), renderOptions);

    renderManager =
        std::make_unique<rendering::RenderManager>(*device, *window, std::move(swapchain), std::move(renderer));
}

void Application::enableInstanceExtension(const char *extensionName)
{
    instanceExtensions.push_back(extensionName);
}

void Application::enableDeviceExtension(const char *extensionName)
{
    deviceExtensions.push_back(extensionName);
}

void Application::enableInstanceLayer(const char *layerName)
{
    instanceLayers.push_back(layerName);
}
} // namespace vkf::platform
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
#include "../core/Buffer.h"
#include "../core/Device.h"
#include "../core/Framebuffer.h"
#include "../core/Instance.h"
#include "../core/PhysicalDevice.h"
#include "../core/Pipeline.h"
#include "../core/RenderPass.h"
#include "../core/Shader.h"
#include "../core/Swapchain.h"
#include "../rendering/BindlessManager.h"
#include "../rendering/ForwardSubstage.h"
#include "../rendering/FrameData.h"
#include "../rendering/GuiSubstage.h"
#include "../rendering/PipelineBuilder.h"
#include "../rendering/RenderManager.h"
#include "../rendering/Renderer.h"
#include "../scene/Camera.h"
#include "../scene/Entity.h"
#include "../scene/Scene.h"
#include "Gui.h"
#include "Window.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <glm/gtc/type_ptr.hpp>
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
        createBindlessManager();
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
    auto frame = renderManager->beginFrame();
    gui->preRender(frame, *scene);

    bindlessManager->updateDescriptorSet();
    scene->getCamera()->updateCameraBuffer();

    renderManager->render();
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

    // For macOS
    enableInstanceExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

#if !defined(NDEBUG)
    enableInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    enableInstanceLayer("VK_LAYER_KHRONOS_validation");
//    enableInstanceLayer("VK_LAYER_LUNARG_monitor");
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

void Application::createScene(const core::RenderPass &renderPass)
{

    vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer buffer{*device, bufferModelCreateInfo,
                        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};
    auto cameraHandle = bindlessManager->storeBuffer(buffer, vk::BufferUsageFlagBits::eUniformBuffer);

    auto camera = scene::Camera{*bindlessManager, cameraHandle, 45, 800, 0.1, 100};

    auto viewProjection = camera.getViewProjectionMatrix();

    //    bindlessManager->updateBuffer(cameraHandle, glm::value_ptr(viewProjection), sizeof(glm::mat4), 0);

    scene = std::make_unique<scene::Scene>(*device, *bindlessManager, renderPass, camera);
}

void Application::createBindlessManager()
{
    bindlessManager = std::make_unique<rendering::BindlessManager>(*device);
}

void Application::createRenderManager()
{
    swapchain = std::make_shared<core::Swapchain>(*device, *surface, *window);

    std::vector<vk::AttachmentDescription> guiAttachments;
    guiAttachments.emplace_back(vk::AttachmentDescription{
        .format = swapchain->selectSwapSurfaceFormat().format, // Getting the image format from the swapchain
        .samples = vk::SampleCountFlagBits::e1,                // Single sample, as multi-sampling is not used
        .loadOp = vk::AttachmentLoadOp::eClear,                // Clear the image at the start
        .storeOp = vk::AttachmentStoreOp::eStore,              // Store the image to memory after rendering
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,      // We don't care about stencil
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,    // We don't care about stencil
        .initialLayout = vk::ImageLayout::eUndefined,          // We don't care about the initial layout
        .finalLayout = vk::ImageLayout::ePresentSrcKHR         // Image will be used as source for presentation
    });

    std::vector<vk::ClearValue> guiClearValues{vk::ClearValue{}};
    guiClearValues[0].color =
        vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}); // Color attachment clear value

    rendering::RenderOptions guiRenderOptions{
        .clearValues = guiClearValues, .numSubpasses = 1, .attachments = guiAttachments, .useDepth = false};

    auto guiRenderer = std::make_unique<rendering::Renderer>(*device, std::move(guiRenderOptions), swapchain);
    gui =
        std::make_shared<Gui>(*window, *instance, *device, *guiRenderer->getRenderPass(), *swapchain, *bindlessManager);
    guiRenderer->addRenderSubstage(std::make_unique<rendering::GuiSubstage>(gui.get()));

    std::vector<vk::AttachmentDescription> sceneAttachments;
    sceneAttachments.emplace_back(vk::AttachmentDescription{
        .format = vk::Format::eR8G8B8A8Srgb,                   // Assuming the image format is R8G8B8A8 srgb
        .samples = vk::SampleCountFlagBits::e1,                // Single sample, as multi-sampling is not used
        .loadOp = vk::AttachmentLoadOp::eClear,                // Clear the image at the start
        .storeOp = vk::AttachmentStoreOp::eStore,              // Store the image to memory after rendering
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,      // We don't care about stencil
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,    // We don't care about stencil
        .initialLayout = vk::ImageLayout::eUndefined,          // We don't care about the initial layout
        .finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal // Image will be used as source for ImGui
    });
    sceneAttachments.emplace_back(vk::AttachmentDescription{
        .format = vk::Format::eD32Sfloat,                   // Assuming the image format is D32 sfloat
        .samples = vk::SampleCountFlagBits::e1,             // Single sample, as multi-sampling is not used
        .loadOp = vk::AttachmentLoadOp::eClear,             // Clear the image at the start
        .storeOp = vk::AttachmentStoreOp::eDontCare,        // Don't store the image to memory after rendering
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,   // We don't care about stencil
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare, // We don't care about stencil
        .initialLayout = vk::ImageLayout::eUndefined,       // We don't care about the initial layout
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal // Image will be used as source for ImGui
    });

    std::vector<vk::ClearValue> sceneClearValues{vk::ClearValue{}, vk::ClearValue{}};
    sceneClearValues[0].color =
        vk::ClearColorValue(std::array<float, 4>{0.05f, 0.05f, 0.05f, 1.0f}); // Color attachment clear value
    sceneClearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);   // Depth attachment clear value

    rendering::RenderOptions sceneRenderOptions{
        .clearValues = sceneClearValues, .numSubpasses = 1, .attachments = sceneAttachments, .useDepth = true};

    auto sceneRenderer = std::make_unique<rendering::Renderer>(*device, std::move(sceneRenderOptions), gui);
    createScene(*sceneRenderer->getRenderPass());
    sceneRenderer->addRenderSubstage(std::make_unique<rendering::ForwardSubstage>(*scene, gui.get(), *bindlessManager));

    std::vector<std::unique_ptr<rendering::Renderer>> renderers;
    renderers.push_back(std::move(sceneRenderer));
    renderers.push_back(std::move(guiRenderer));

    renderManager = std::make_unique<rendering::RenderManager>(*device, *window, swapchain, std::move(renderers));
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
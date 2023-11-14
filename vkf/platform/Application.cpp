/// \file
/// \brief

//
// Created by Joshua Lowe on 10/30/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Application.h"
#include "Window.h"
#include "../core/Instance.h"
#include "../common/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace vkf::platform {
    Application::Application(const std::string &appName) {

      window = std::make_unique<Window>(
        Window::Properties{.title{"VulkanRenderer"}, .mode=Window::Mode::Windowed, .resizeable=true, .extent{
          1000, 1000}});

      window->setEventCallback([this](auto &&arg) { onEvent(std::forward<decltype(arg)>(arg)); });

      auto surfaceExtensions = window->getRequiredSurfaceExtensions();
      for (const char *extensionName: surfaceExtensions) {
        instanceExtensions.push_back(extensionName);
      }

      instance = std::make_unique<core::Instance>(appName, instanceExtensions);

      surface = std::make_unique<vk::raii::SurfaceKHR>(window->createSurface(*instance));

      if (!surface) {
        throw std::runtime_error("Failed to create window surface.");
      }

      device = std::make_unique<core::Device>(*instance, *surface);
    }

    void Application::run() {
      while (!window->isClosed()) {
        onUpdate();
        window->onUpdate();
//    drawFrame();
      }
    }

    void Application::onEvent(Event &event) {
      switch (event.type) {
        case Event::Type::Keyboard: {
          auto data = std::get<Event::Keyboard>(event.data);
          LOG_DEBUG("KeyboardEvent: keycode={}, action={}", data.keycode, data.action);
          break;
        }
        case Event::Type::MouseMove: {
          auto data = std::get<Event::MouseMove>(event.data);
          LOG_DEBUG("MouseMoveEvent: x={}, y={}", data.xPos, data.yPos);
          break;
        }
        case Event::Type::MouseButton: {
          auto data = std::get<Event::MouseButton>(event.data);
          LOG_DEBUG("MouseButtonEvent: button={}, action={}", data.button, data.action);
          break;
        }
        case Event::Type::MouseScroll: {
          auto data = std::get<Event::MouseScroll>(event.data);
          LOG_DEBUG("MouseScrollEvent: x={}, y={}", data.xScroll, data.yScroll);
          break;
        }
        case Event::Type::Resize: {
          auto data = std::get<Event::Resize>(event.data);
          LOG_DEBUG("ResizeEvent: width={}, height={}", data.newWidth, data.newHeight);
          break;
        }
        case Event::Type::Close: {
          LOG_DEBUG("CloseEvent");
          break;
        }
      }
    }

    void Application::onUpdate() {

    }

    void Application::initLogger() {
      try {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
        auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
#if !defined( NDEBUG )
        logger->
          set_level(spdlog::level::debug);
#else
        logger->set_level(spdlog::level::info);
#endif
        spdlog::set_pattern("[%^%l%$] %v");
        spdlog::set_default_logger(logger);
        logger->info("Initialized logger");
      } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
      }
    }
} // namespace vkf::platform

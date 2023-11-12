/// \file
/// \brief

//
// Created by Joshua Lowe on 10/30/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "../pch.h"
#include "Application.h"
#include "Window.h"
#include "../core/Instance.h"
#include "../common/logging.h"

#include "../common/input.h"

namespace vkf {
    Application::Application(const std::string &appName) {

      window = std::make_unique<Window>(
        Window::Properties{.title{"VulkanRenderer"}, .mode=Window::Mode::Windowed, .resizeable=true, .extent{
          1000, 1000}});

      window->setEventCallback([this](auto &&arg) { onEvent(std::forward<decltype(arg)>(arg)); });

      for (const char *extensionName: Window::getRequiredSurfaceExtensions()) {
        instanceExtensions.push_back(extensionName);
      }

      instance = std::make_unique<core::Instance>(appName, instanceExtensions);

      surface = std::make_unique<vk::raii::SurfaceKHR>(window->createSurface(*instance));

      if (!surface)
      {
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
          LOG_INFO("KeyboardEvent: keycode={}, action={}", data.keycode, data.action);
          break;
        }
        case Event::Type::MouseMove: {
          auto data = std::get<Event::MouseMove>(event.data);
          LOG_INFO("MouseMoveEvent: x={}, y={}", data.xPos, data.yPos);
          break;
        }
        case Event::Type::MouseButton: {
          auto data = std::get<Event::MouseButton>(event.data);
          LOG_INFO("MouseButtonEvent: button={}, action={}", data.button, data.action);
          break;
        }
        case Event::Type::MouseScroll: {
          auto data = std::get<Event::MouseScroll>(event.data);
          LOG_INFO("MouseScrollEvent: x={}, y={}", data.xScroll, data.yScroll);
          break;
        }
        case Event::Type::Resize: {
          auto data = std::get<Event::Resize>(event.data);
          LOG_INFO("ResizeEvent: width={}, height={}", data.newWidth, data.newHeight);
          break;
        }
        case Event::Type::Close: {
          LOG_INFO("CloseEvent");
          break;
        }
      }
    }

    void Application::onUpdate() {

    }
} // namespace vkf

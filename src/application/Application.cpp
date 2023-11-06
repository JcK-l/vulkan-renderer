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
    Application::Application(const std::string &appName) : instance{appName} {
      window = std::make_unique<Window>(
        Window::Properties{.title{"VulkanRenderer"}, .mode=Window::Mode::Windowed, .resizeable=true, .extent{
          1000, 1000}});
      window->setEventCallback([this](auto &&arg) { onEvent(std::forward<decltype(arg)>(arg)); });
    }

    void Application::run() {
      while (!window->isClosed()) {
        onUpdate();
        window->onUpdate();
//    drawFrame();
      }
    }

    void Application::initVulkan() {

    }

    void Application::onEvent(Event &event) {
      switch (event.type) {
        case Event::Type::Keyboard: {
          auto data = std::get<Event::Keyboard>(event.data);
          LOGI("KeyboardEvent: keycode={}, action={}", data.keycode, data.action);
          break;
        }
        case Event::Type::MouseMove: {
          auto data = std::get<Event::MouseMove>(event.data);
          LOGI("MouseMoveEvent: x={}, y={}", data.xPos, data.yPos);
          break;
        }
        case Event::Type::MouseButton: {
          auto data = std::get<Event::MouseButton>(event.data);
          LOGI("MouseButtonEvent: button={}, action={}", data.button, data.action);
          break;
        }
        case Event::Type::MouseScroll: {
          auto data = std::get<Event::MouseScroll>(event.data);
          LOGI("MouseScrollEvent: x={}, y={}", data.xScroll, data.yScroll);
          break;
        }
        case Event::Type::Resize: {
          auto data = std::get<Event::Resize>(event.data);
          LOGI("ResizeEvent: width={}, height={}", data.newWidth, data.newHeight);
          break;
        }
        case Event::Type::Close: {
          LOGI("CloseEvent");
          break;
        }
      }
    }

    void Application::onUpdate() {

    }
} // namespace vkf

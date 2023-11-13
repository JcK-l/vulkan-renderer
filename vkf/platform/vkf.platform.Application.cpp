/// \file
/// \brief

//
// Created by Joshua Lowe on 11/13/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include "../common/Log.h"
#include "../common/Event.h"
#include <vector>
#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

module vkf.platform.Application;

import vkf.core.Instance;
import vkf.core.Device;
import vkf.platform.Window;

namespace vkf::platform {
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

    void Application::onEvent(common::Event &event) {
      switch (event.type) {
        case common::Event::Type::Keyboard: {
          auto data = std::get<common::Event::Keyboard>(event.data);
          LOG_INFO("KeyboardEvent: keycode={}, action={}", data.keycode, data.action);
          break;
        }
        case common::Event::Type::MouseMove: {
          auto data = std::get<common::Event::MouseMove>(event.data);
          LOG_INFO("MouseMoveEvent: x={}, y={}", data.xPos, data.yPos);
          break;
        }
        case common::Event::Type::MouseButton: {
          auto data = std::get<common::Event::MouseButton>(event.data);
          LOG_INFO("MouseButtonEvent: button={}, action={}", data.button, data.action);
          break;
        }
        case common::Event::Type::MouseScroll: {
          auto data = std::get<common::Event::MouseScroll>(event.data);
          LOG_INFO("MouseScrollEvent: x={}, y={}", data.xScroll, data.yScroll);
          break;
        }
        case common::Event::Type::Resize: {
          auto data = std::get<common::Event::Resize>(event.data);
          LOG_INFO("ResizeEvent: width={}, height={}", data.newWidth, data.newHeight);
          break;
        }
        case common::Event::Type::Close: {
          LOG_INFO("CloseEvent");
          break;
        }
      }
    }

    void Application::onUpdate() {

    }
} // namespace vkf::platform

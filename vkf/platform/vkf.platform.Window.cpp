/// \file
/// \brief

//
// Created by Joshua Lowe on 11/13/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include "GLFW/glfw3.h"
#include "../common/Event.h"

#include <utility>
#include <vector>
#include <functional>
#include "../common/Log.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

module vkf.platform.Window;

import vkf.core.Instance;

void errorCallback(int error, const char *description) {
  LOG_ERROR("GLFW ({}): {}", error, description);
}

namespace vkf::platform {
    Window::Window(Window::Properties properties) : windowData(std::move(properties)) {

      if (!glfwInit()) {
        throw std::runtime_error("GLFW couldn't be initialized.");
      }

      glfwSetErrorCallback(errorCallback);
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, windowData.resizeable);

      switch (windowData.mode) {
        case Window::Mode::Fullscreen: {
          auto *monitor = glfwGetPrimaryMonitor();
          const auto *mode = glfwGetVideoMode(monitor);
          handle = glfwCreateWindow(mode->width, mode->height, windowData.title.c_str(), monitor, nullptr);
          break;
        }
        case Window::Mode::FullscreenBorderless: {
          auto *monitor = glfwGetPrimaryMonitor();
          const auto *mode = glfwGetVideoMode(monitor);
          glfwWindowHint(GLFW_RED_BITS, mode->redBits);
          glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
          glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
          glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
          handle = glfwCreateWindow(mode->width, mode->height, windowData.title.c_str(), monitor, nullptr);
          break;
        }
        case Window::Mode::Windowed:
          handle = glfwCreateWindow(windowData.extent.width, windowData.extent.height, windowData.title.c_str(),
                                    nullptr,
                                    nullptr);
          break;
      }

      if (!handle) {
        throw std::runtime_error("Couldn't create glfw window.");
      }

      glfwSetWindowUserPointer(handle, &windowData);
      glfwSetWindowSizeCallback(handle, [](GLFWwindow *window, int width, int height) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            data->extent.width = width;
            data->extent.height = height;
            data->resized = true;
            common::Event event{common::Event::Type::Resize, common::Event::Resize{width, height}};
            data->eventCallback(event);
          }
      });

      glfwSetWindowCloseCallback(handle, [](GLFWwindow *window) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            common::Event event{common::Event::Type::Close, common::Event::Close{}};
            data->eventCallback(event);
          }
          glfwSetWindowShouldClose(window, GLFW_TRUE);
      });

      glfwSetKeyCallback(handle, [](GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            common::Event event{common::Event::Type::Keyboard, common::Event::Keyboard{key, action}};
            data->eventCallback(event);
          }
      });

      glfwSetCursorPosCallback(handle, [](GLFWwindow *window, double xPos, double yPos) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            common::Event event{common::Event::Type::MouseMove, common::Event::MouseMove{xPos, yPos}};
            data->eventCallback(event);
          }
      });

      glfwSetMouseButtonCallback(handle, [](GLFWwindow *window, int button, int action, int /*mods*/) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            common::Event event{common::Event::Type::MouseButton, common::Event::MouseButton{button, action}};
            data->eventCallback(event);
          }
      });

      glfwSetScrollCallback(handle, [](GLFWwindow *window, double xScroll, double yScroll) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            common::Event event{common::Event::Type::MouseScroll, common::Event::MouseScroll{xScroll, yScroll}};
            data->eventCallback(event);
          }
      });
    }

    Window::~Window() {
      glfwDestroyWindow(handle);
      glfwTerminate();
    }

    void Window::onUpdate() {
      glfwPollEvents();
    }

    void Window::setEventCallback(const std::function<void(common::Event &)> &callback) {
      windowData.eventCallback = callback;
    }

    bool Window::isClosed() const {
      return glfwWindowShouldClose(handle);
    }

    GLFWwindow *Window::getHandle() const {
      return handle;
    }

    std::vector<const char *> Window::getRequiredSurfaceExtensions() {
      uint32_t glfwExtensionCount{0};
      const char **names = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
      std::vector<const char *> extensions{names, names + glfwExtensionCount};
      return {names, names + glfwExtensionCount};
    }

    vk::raii::SurfaceKHR Window::createSurface(const core::Instance &instance) {
      VkSurfaceKHR _surface;
      glfwCreateWindowSurface(static_cast<VkInstance>( *instance.getHandle()), handle, nullptr, &_surface);
      vk::raii::SurfaceKHR surface{instance.getHandle(), _surface};
      return surface;
    };
} // namespace vkf::platform
/// \file
/// \brief

//
// Created by Joshua Lowe on 10/31/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "../pch.h"
#include "Window.h"

#include <utility>
#include "GLFW/glfw3.h"
#include "Application.h"
#include "../common/logging.h"

void errorCallback(int error, const char *description) {
  LOGE("GLFW ({}): {}", error, description);
}

namespace vkf {
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
            Event event{Event::Type::Resize, Event::Resize{width, height}};
            data->eventCallback(event);
          }
      });

      glfwSetWindowCloseCallback(handle, [](GLFWwindow *window) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            Event event{Event::Type::Close, Event::Close{}};
            data->eventCallback(event);
          }
          glfwSetWindowShouldClose(window, GLFW_TRUE);
      });

      glfwSetKeyCallback(handle, [](GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            Event event{Event::Type::Keyboard, Event::Keyboard{key, action}};
            data->eventCallback(event);
          }
      });

      glfwSetCursorPosCallback(handle, [](GLFWwindow *window, double xPos, double yPos) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            Event event{Event::Type::MouseMove, Event::MouseMove{xPos, yPos}};
            data->eventCallback(event);
          }
      });

      glfwSetMouseButtonCallback(handle, [](GLFWwindow *window, int button, int action, int /*mods*/) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            Event event{Event::Type::MouseButton, Event::MouseButton{button, action}};
            data->eventCallback(event);
          }
      });

      glfwSetScrollCallback(handle, [](GLFWwindow *window, double xScroll, double yScroll) {
          if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
            Event event{Event::Type::MouseScroll, Event::MouseScroll{xScroll, yScroll}};
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
} // namespace vkf

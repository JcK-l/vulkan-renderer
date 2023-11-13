/// \file
/// \brief

//
// Created by Joshua Lowe on 11/13/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include "GLFW/glfw3.h"
#include <utility>
//#include "../../src/pch.h"


export module vkf.common.input;

/// \brief Collection of input functions
namespace vkf::common::input {
    bool isKeyPressed(GLFWwindow *window, int keycode);

    bool isMouseButtonPressed(GLFWwindow *window, int button);

    std::pair<double, double> getMousePosition(GLFWwindow *window);

    double getMouseX(GLFWwindow *window);

    double getMouseY(GLFWwindow *window);
} // vkf::common::input

//@formatter:off
module : private;
//@formatter:on

namespace vkf::common::input {
    bool isKeyPressed(GLFWwindow *window, int keycode) {
      auto state = glfwGetKey(window, keycode);
      return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool isMouseButtonPressed(GLFWwindow *window, int button) {
      auto state = glfwGetMouseButton(window, button);
      return state == GLFW_PRESS;
    }

    std::pair<double, double> getMousePosition(GLFWwindow *window) {
      double xPos;
      double yPos;
      glfwGetCursorPos(window, &xPos, &yPos);
      return {xPos, yPos};
    }

    double getMouseX(GLFWwindow *window) {
      auto [x, y] = getMousePosition(window);
      return x;
    }

    double getMouseY(GLFWwindow *window) {
      auto [x, y] = getMousePosition(window);
      return y;
    }
} // vkf::common::input

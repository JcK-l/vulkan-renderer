////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Input.cpp
/// \brief This file implements functions for handling input events.
///
/// The functions in this file are part of the vkf namespace. They provide an interface for checking the state of
/// keyboard and mouse input. These functions use the GLFW library to check the state of the keyboard and mouse.
///
/// \author Joshua Lowe
/// \date 11/6/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "input.h"
#include "GLFW/glfw3.h"

namespace vkf
{
bool isKeyPressed(GLFWwindow *window, int keycode)
{
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool isMouseButtonPressed(GLFWwindow *window, int button)
{
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<double, double> getMousePosition(GLFWwindow *window)
{
    double xPos;
    double yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return {xPos, yPos};
}

double getMouseX(GLFWwindow *window)
{
    auto [x, y] = getMousePosition(window);
    return x;
}

double getMouseY(GLFWwindow *window)
{
    auto [x, y] = getMousePosition(window);
    return y;
}
} // namespace vkf
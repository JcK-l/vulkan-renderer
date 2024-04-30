////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Input.h
/// \brief This file declares functions for handling input events.
///
/// The functions in this file are part of the vkf namespace. They provide an interface for checking the state of
/// keyboard and mouse input.
///
/// \author Joshua Lowe
/// \date 11/6/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class GLFWwindow;

namespace vkf
{
bool isKeyPressed(GLFWwindow *window, int keycode);

bool isMouseButtonPressed(GLFWwindow *window, int button);

std::pair<double, double> getMousePosition(GLFWwindow *window);

double getMouseX(GLFWwindow *window);

double getMouseY(GLFWwindow *window);
} // namespace vkf
/// \file
/// \brief

//
// Created by Joshua Lowe on 11/6/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_INPUT_H
#define VULKANRENDERER_INPUT_H

class GLFWwindow;

/// \brief Collection of input functions
namespace vkf::input {
    bool isKeyPressed(GLFWwindow *window, int keycode);

    bool isMouseButtonPressed(GLFWwindow *window, int button);

    std::pair<double, double> getMousePosition(GLFWwindow *window);

    double getMouseX(GLFWwindow *window);

    double getMouseY(GLFWwindow *window);
}

#endif //VULKANRENDERER_INPUT_H

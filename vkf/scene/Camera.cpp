/// \file
/// \brief

//
// Created by Joshua Lowe on 12/18/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Camera.h"
#include "../rendering/BindlessManager.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

Camera::Camera(vkf::rendering::BindlessManager &bindlessManager, uint32_t handle, float fov, float aspect, float near,
               float far)
    : bindlessManager{bindlessManager}, handle{handle}, position{0.0f, 0.0f, 2.0f}, target{0.0f, 0.0f, 0.0f},
      up{0.0f, 1.0f, 0.0f}
{
    view = glm::lookAt(position, target, up);
    projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

const glm::mat4 &Camera::getViewMatrix() const
{
    return view;
}

const glm::mat4 &Camera::getProjectionMatrix() const
{
    return projection;
}

uint32_t Camera::getHandle() const
{
    return handle;
}

glm::mat4 Camera::getViewProjectionMatrix() const
{
    return projection * view;
}

void Camera::orbit(float dx, float dy)
{
    // Convert the camera's position to spherical coordinates
    float radius = glm::length(position - target);
    float theta = std::atan2(position.z - target.z, position.x - target.x);
    float phi = std::acos((position.y - target.y) / radius);

    // Adjust theta and phi based on the mouse movement
    // Note: The 0.01f is a sensitivity factor that you can adjust as needed
    // Negate dx and dy to invert the controls
    float newTheta = theta - dx * 0.01f;
    float newPhi = phi + dy * 0.01f;

    // Clamp phi to avoid flipping the camera upside down
    newPhi = glm::clamp(newPhi, 0.1f, 3.14f);

    // Apply a lerp function to smooth the transition
    float smoothFactor = 0.4f; // Adjust this value to change the smoothness
    theta = theta + smoothFactor * (newTheta - theta);
    phi = phi + smoothFactor * (newPhi - phi);

    // Convert the spherical coordinates back to Cartesian coordinates
    position.x = target.x + radius * std::sin(phi) * std::cos(theta);
    position.y = target.y + radius * std::cos(phi);
    position.z = target.z + radius * std::sin(phi) * std::sin(theta);

    updateViewMatrix();
}

void Camera::zoom(float distance)
{
    float radius = glm::length(position - target);
    float newRadius = radius + distance;

    // Set limits for the radius
    float minRadius = 2.0f;   // Minimum distance from the target
    float maxRadius = 100.0f; // Maximum distance from the target

    // Clamp the new radius to the [minRadius, maxRadius] range
    newRadius = std::clamp(newRadius, minRadius, maxRadius);

    // Apply a lerp function to smooth the transition
    float smoothFactor = 0.4f; // Adjust this value to change the smoothness
    newRadius = radius + smoothFactor * (newRadius - radius);

    glm::vec3 direction = glm::normalize(target - position);
    position = target - direction * newRadius;

    updateViewMatrix();
}

void Camera::updateAspectRatio(float aspect)
{
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Camera::updateCameraBuffer()
{
    bindlessManager.updateBuffer(handle, glm::value_ptr(getViewProjectionMatrix()), sizeof(glm::mat4), 0);
}

void Camera::updateViewMatrix()
{
    view = glm::lookAt(position, target, up);
}

} // namespace vkf::scene
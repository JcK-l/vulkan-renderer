////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Camera.cpp
/// \brief This file implements the Camera class which is used for managing camera data in a scene.
///
/// The Camera class is part of the vkf::scene namespace. It provides functionality to store and manage camera data.
/// It also provides methods to get view and projection matrices, orbit, zoom, update aspect ratio, and update camera
/// buffer.
///
/// \author Joshua Lowe
/// \date 12/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Camera.h"
#include "../common/Log.h"
#include "../rendering/BindlessManager.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

Camera::Camera(vkf::rendering::BindlessManager &bindlessManager, uint32_t handle, float fov, float aspect, float near,
               float far)
    : bindlessManager{bindlessManager}, handle{handle}, position{0.0f, 400.0f, 400.0f}, target{0.0f, 0.0f, 0.0f},
      up{0.0f, 1.0f, 0.0f}, fov{fov}, aspect{aspect}, near{near}, far{far}
{
    createViewMatrix();
    createProjectionMatrix();

    LOG_INFO("Camera created")
}

const glm::mat4 &Camera::getViewMatrix() const
{
    return view;
}

const glm::mat4 &Camera::getProjectionMatrix() const
{
    return projection;
}

const glm::mat4 &Camera::getViewMatrixFlip() const
{
    return viewFlip;
}

const glm::mat4 &Camera::getProjectionMatrixFlip() const
{
    return projectionFlip;
}

glm::vec3 Camera::getXAxis() const
{
    return glm::vec3{view[0][0], view[1][0], view[2][0]};
}

glm::vec3 Camera::getPosition() const
{
    return position;
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
    float newTheta = theta + dx * 0.01f;
    float newPhi = phi - dy * 0.01f;

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

    createViewMatrix();
}

void Camera::zoom(float distance)
{
    float radius = glm::length(position - target);
    float newRadius = radius + distance * zoomSpeed;

    // Set limits for the radius
    float minRadius = 60.0f;  // Minimum distance from the target
    float maxRadius = 600.0f; // Maximum distance from the target

    // Clamp the new radius to the [minRadius, maxRadius] range
    newRadius = std::clamp(newRadius, minRadius, maxRadius);

    // Apply a lerp function to smooth the transition
    float smoothFactor = 0.6f; // Adjust this value to change the smoothness
    newRadius = radius + smoothFactor * (newRadius - radius);

    glm::vec3 direction = glm::normalize(target - position);
    position = target - direction * newRadius;

    createViewMatrix();
}

void Camera::updateAspectRatio(float aspect)
{
    //    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    this->aspect = aspect;
    projectionFlip = glm::perspectiveRH(glm::radians(fov), aspect, near, far);
    //    projection[0][0] = 1.0f / std::tan(aspect * (glm::radians(fov) / 2.0f));
    projection = glm::perspectiveLH_ZO(glm::radians(fov), aspect, near, far);
}

void Camera::updateCameraBuffer()
{
    bindlessManager.updateBuffer(handle, glm::value_ptr(getViewProjectionMatrix()), sizeof(glm::mat4), 0);
}

void Camera::createProjectionMatrix()
{
    projectionFlip = glm::perspectiveLH_ZO(glm::radians(fov), aspect, near, far);
    //    projection = glm::mat4{0.0f};
    //    projection[0][0] = 1.0f / std::tan(aspect * (glm::radians(fov) / 2.0f));
    //    projection[1][1] = 1.0f / std::tan(glm::radians(fov) / 2.0f);
    //    projection[2][2] = far / (far - near);
    //    projection[2][3] = 1.0f;
    //    projection[3][2] = -(far * near) / (far - near);
    projection = glm::perspectiveLH_ZO(glm::radians(fov), aspect, near, far);
}

void Camera::createViewMatrix()
{
    viewFlip = glm::lookAtRH(position, target, up);
    const glm::vec3 direction = glm::normalize(target - position);
    const glm::vec3 axis1 = glm::normalize(glm::cross(direction, up));
    const glm::vec3 axis2 = glm::cross(direction, axis1);
    view = glm::mat4{1.0f};
    view[0][0] = axis1.x;
    view[1][0] = axis1.y;
    view[2][0] = axis1.z;
    view[0][1] = axis2.x;
    view[1][1] = axis2.y;
    view[2][1] = axis2.z;
    view[0][2] = direction.x;
    view[1][2] = direction.y;
    view[2][2] = direction.z;
    view[3][0] = -glm::dot(axis1, position);
    view[3][1] = -glm::dot(axis2, position);
    view[3][2] = -glm::dot(direction, position);
}

} // namespace vkf::scene
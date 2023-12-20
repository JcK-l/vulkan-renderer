/// \file
/// \brief

//
// Created by Joshua Lowe on 12/18/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_CAMERA_H
#define VULKANRENDERER_CAMERA_H

#include <glm/glm.hpp>

namespace vkf::rendering // Forward declarations
{
class BindlessManager;
} // namespace vkf::rendering

namespace vkf::scene
{

class Camera
{
  public:
    Camera(rendering::BindlessManager &bindlessManager, uint32_t handle, float fov, float aspect, float near,
           float far);

    [[nodiscard]] const glm::mat4 &getViewMatrix() const;
    [[nodiscard]] const glm::mat4 &getProjectionMatrix() const;
    [[nodiscard]] glm::mat4 getViewProjectionMatrix() const;
    [[nodiscard]] uint32_t getHandle() const;

    void orbit(float dx, float dy);

    void zoom(float distance);

    void updateAspectRatio(float aspect);

    void updateCameraBuffer();

  private:
    void updateViewMatrix();

    rendering::BindlessManager &bindlessManager;
    uint32_t handle;

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    glm::mat4 view;
    glm::mat4 projection;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_CAMERA_H
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Camera.h
/// \brief This file declares the Camera class which is used for managing camera data in a scene.
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

#ifndef VULKANRENDERER_CAMERA_H
#define VULKANRENDERER_CAMERA_H

#include <glm/glm.hpp>

namespace vkf::rendering // Forward declarations
{
class BindlessManager;
} // namespace vkf::rendering

namespace vkf::scene
{

///
/// \class Camera
/// \brief Class for managing camera data in a scene.
///
/// This class provides functionality to store and manage camera data. It contains a BindlessManager reference and a
/// handle to manage bindless resources, glm::vec3 members to store the position, target, and up vector data, and
/// glm::mat4 members to store the view and projection matrices.
///
class Camera
{
  public:
    ///
    /// \brief Constructor that takes a BindlessManager reference, handle, field of view, aspect ratio, near and far
    /// plane distances as parameters.
    ///
    /// This constructor initializes the bindlessManager reference, handle, and camera parameters with the provided
    /// values.
    ///
    /// \param bindlessManager The BindlessManager reference to use for creating the Camera.
    /// \param handle The handle to use for creating the Camera.
    /// \param fov The field of view to use for creating the Camera.
    /// \param aspect The aspect ratio to use for creating the Camera.
    /// \param near The near plane distance to use for creating the Camera.
    /// \param far The far plane distance to use for creating the Camera.
    ///
    Camera(rendering::BindlessManager &bindlessManager, uint32_t handle, float fov, float aspect, float near,
           float far);

    Camera(const Camera &) = delete;            ///< Deleted copy constructor
    Camera(Camera &&) noexcept = default;       ///< Default move constructor
    Camera &operator=(const Camera &) = delete; ///< Deleted copy assignment operator
    Camera &operator=(Camera &&) = delete;      ///< Deleted move assignment operator
    ~Camera() = default;                        ///< Default destructor

    [[nodiscard]] const glm::mat4 &getViewMatrix() const;
    [[nodiscard]] const glm::mat4 &getProjectionMatrix() const;
    [[nodiscard]] glm::mat4 getViewProjectionMatrix() const;
    [[nodiscard]] uint32_t getHandle() const;

    /// \brief Method to orbit the camera.
    ///
    /// This method takes the change in x and y directions and orbits the camera.
    void orbit(float dx, float dy);

    /// \brief Method to zoom the camera.
    ///
    /// This method takes the distance and zooms the camera.
    void zoom(float distance);

    /// \brief Method to update the aspect ratio.
    ///
    /// This method takes the aspect ratio and updates the camera's aspect ratio.
    void updateAspectRatio(float aspect);

    /// \brief Method to update the camera buffer.
    ///
    /// This method updates the camera buffer.
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
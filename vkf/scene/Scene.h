////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Scene.h
/// \brief This file declares the Scene class and the TagComponent struct which are used for managing a scene.
///
/// The Scene class and the TagComponent struct are part of the vkf::scene namespace.
/// It provides an interface for interacting with a scene.
///
/// \author Joshua Lowe
/// \date 12/3/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_SCENE_H
#define VULKANRENDERER_SCENE_H

#include "components/ColorComponent.h"
#include "components/MaterialComponent.h"
#include "components/TagComponent.h"
#include "components/TransformComponent.h"
#include <entt/entt.hpp>

namespace vkf::core // Forward declarations
{
class Device;
class RenderPass;
} // namespace vkf::core

namespace vkf::rendering // Forward declarations
{
class PipelineBuilder;
class BindlessManager;
} // namespace vkf::rendering

namespace vkf::scene
{

class Entity;
class Camera;

///
/// \class Scene
/// \brief This class manages a scene.
///
/// It provides an interface for interacting with with a scene.
/// It includes methods for creating an entity and getting the registry.
///
class Scene
{
  public:
    ///
    /// \brief Constructor for the Scene class.
    ///
    /// This constructor creates a Scene.
    ///
    explicit Scene(const core::Device &device, rendering::BindlessManager &bindlessManager,
                   const core::RenderPass &renderPass, Camera &camera);
    ~Scene();

    Entity createCube(const std::string &name, const glm::vec3 &position, const glm::vec3 &rotation,
                      const glm::vec3 &scale, const glm::vec4 &color);

    entt::registry &getRegistry();
    [[nodiscard]] Camera *getCamera() const;

  private:
    Entity createObject(const std::string &name, const glm::vec3 &position, const glm::vec3 &rotation,
                        const glm::vec3 &scale, const glm::vec4 &color, const std::string &shader);

    Entity createEntity(const std::string &name);
    core::Pipeline createPipeline(const std::string &shader);

    const core::Device &device;
    rendering::BindlessManager &bindlessManager;
    const core::RenderPass &renderPass;

    std::unique_ptr<Camera> sceneCamera;

    entt::registry registry;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_SCENE_H
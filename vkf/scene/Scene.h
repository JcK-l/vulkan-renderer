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

#include "components/Components.h"
#include "prefabs/PrefabFactory.h"
#include <entt/entt.hpp>

namespace vkf::core // Forward declarations
{
class Device;
class RenderPass;
class Pipeline;
} // namespace vkf::core

namespace vkf::rendering // Forward declarations
{
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
    /// \param device The device to use.
    /// \param bindlessManager The bindless manager to use.
    /// \param renderPass The render pass to use.
    /// \param camera The camera to use.
    ///
    explicit Scene(const core::Device &device, rendering::BindlessManager &bindlessManager,
                   const core::RenderPass &renderPass, Camera &camera);

    Scene(const Scene &) = delete;            ///< Deleted copy constructor
    Scene(Scene &&) noexcept = default;       ///< Default move constructor
    Scene &operator=(const Scene &) = delete; ///< Deleted copy assignment operator
    Scene &operator=(Scene &&) = delete;      ///< Deleted move assignment operator
    ~Scene();                                 ///< Implemented in Scene.cpp

    std::unique_ptr<Entity> createEntity(PrefabType type, std::string tag);

    entt::registry &getRegistry();
    [[nodiscard]] Camera *getCamera() const;

  private:
    const core::Device &device;
    rendering::BindlessManager &bindlessManager;
    const core::RenderPass &renderPass;

    std::unique_ptr<Camera> sceneCamera;
    std::unique_ptr<PrefabFactory> prefabFactory;

    entt::registry registry;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_SCENE_H
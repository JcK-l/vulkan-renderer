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

#include <utility>

#include "entt/entt.hpp"

namespace vkf::scene
{

///
/// \struct TagComponent
/// \brief This struct stores a tag for an entity.
///
/// It includes a string tag.
///
struct TagComponent
{
    std::string tag;

    TagComponent(std::string tag) : tag{std::move(tag)}
    {
    }
};

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
    Scene();
    ~Scene() = default;

    ///
    /// \brief Method to create an entity.
    ///
    /// This method creates an entity with the provided name.
    ///
    /// \param name The name for the entity.
    /// \return The created entity.
    ///
    entt::entity createEntity(const std::string &name);

    entt::registry &getRegistry();

  private:
    entt::registry registry;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_SCENE_H
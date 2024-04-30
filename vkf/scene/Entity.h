////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Entity.h
/// \brief This file declares the Entity class which is used for managing entities in a scene.
///
/// The Entity class is part of the vkf::scene namespace. It provides functionality to create, destroy, and manage
/// entities. It also provides methods to add components to an entity, get a component from an entity, get and set the
/// handle of an entity.
///
/// \author Joshua Lowe
/// \date 12/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <entt/entt.hpp>

// Forward declarations
#include "../core/CoreFwd.h"
#include "../rendering/RenderingFwd.h"

namespace vkf::scene
{

///
/// \class Entity
/// \brief Class for managing entities in a scene.
///
/// This class provides functionality to create, destroy, and manage entities. It contains a reference to an
/// entt::registry and an entt::entity handle.
///
class Entity
{
  public:
    ///
    /// \brief Constructor that takes an entt::registry reference as parameter.
    ///
    /// This constructor initializes the registry reference with the provided registry.
    ///
    /// \param registry Reference to the entt::registry
    ///
    explicit Entity(entt::registry &registry);

    Entity(const Entity &) = delete;            ///< Deleted copy constructor
    Entity(Entity &&) noexcept = default;       ///< Default move constructor
    Entity &operator=(const Entity &) = delete; ///< Deleted copy assignment operator
    Entity &operator=(Entity &&) noexcept;      ///< Deleted move assignment operator
    ~Entity() = default;                        ///< Default destructor

    ///
    /// \brief Method to add a component to an entity.
    ///
    /// This method takes the type of the component and its arguments, and adds the component to the entity.
    ///
    template <typename T, typename... Args> T &addComponent(Args &&...args)
    {
        assert(handle != entt::null && "Entity is not valid");
        registry.emplace<T>(handle, std::forward<Args>(args)...);
        return registry.get<T>(handle);
    }

    ///
    /// \brief Method to get a component from an entity.
    ///
    /// This method takes the type of the component and returns the component from the entity.
    ///
    template <typename T> T &getComponent()
    {
        assert(handle != entt::null && "Entity is not valid");
        return registry.get<T>(handle);
    }

    [[nodiscard]] entt::entity getHandle() const;

    void create();
    void destroy();

  private:
    entt::registry &registry;
    entt::entity handle{entt::null};
};

} // namespace vkf::scene
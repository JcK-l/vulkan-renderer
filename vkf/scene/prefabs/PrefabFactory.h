////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PrefabFactory.h
/// \brief This file declares the PrefabFactory class, which is used to create prefabricated entities in the vkf::scene
/// namespace.
///
/// The PrefabFactory class is part of the vkf::scene namespace. It provides an interface for creating prefabricated
/// entities, such as Cubes and Triangles. These entities are used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_PREFABFACTORY_H
#define VULKANRENDERER_PREFABFACTORY_H

#include "Cube.h"
#include "Triangle.h"
#include <entt/entt.hpp>

namespace vkf::core // Forward declarations
{
class Device;
class Pipeline;
class RenderPass;
} // namespace vkf::core

namespace vkf::rendering // Forward declarations
{
class BindlessManager;
} // namespace vkf::rendering

namespace vkf::scene
{

// Forward declarations
class Entity;
class Camera;

///
/// \enum PrefabType
/// \brief Enumerates the types of prefabricated entities that can be created.
///
enum class PrefabType
{
    Cube,
    Triangle,
    Custom
};

///
/// \struct PrefabTraits
/// \brief Provides traits for prefabricated entities.
///
template <typename T> struct PrefabTraits;

template <> struct PrefabTraits<Cube>
{
    static PrefabType getPrefabType()
    {
        return PrefabType::Cube;
    }
};

template <> struct PrefabTraits<Triangle>
{
    static PrefabType getPrefabType()
    {
        return PrefabType::Triangle;
    }
};

///
/// \class PrefabFactory
/// \brief This class is used to create prefabricated entities.
///
/// This class provides an interface for creating prefabricated entities, such as Cubes and Triangles. These entities
/// are used in a 3D scene.
///
class PrefabFactory
{
  public:
    ///
    /// \brief Constructs a PrefabFactory object.
    ///
    /// This constructor initializes the PrefabFactory with the provided device, bindlessManager, renderPass, and
    /// camera.
    ///
    /// \param device The Vulkan device to use for creating the prefabricated entities.
    /// \param bindlessManager The BindlessManager to use for creating the prefabricated entities.
    /// \param renderPass The RenderPass to use for creating the prefabricated entities.
    /// \param camera The Camera to use for creating the prefabricated entities.
    ///
    PrefabFactory(const core::Device &device, rendering::BindlessManager &bindlessManager,
                  const core::RenderPass &renderPass, Camera *camera);

    PrefabFactory(const PrefabFactory &) = delete;            ///< Deleted copy constructor
    PrefabFactory(PrefabFactory &&) noexcept = default;       ///< Default move constructor
    PrefabFactory &operator=(const PrefabFactory &) = delete; ///< Deleted copy assignment operator
    PrefabFactory &operator=(PrefabFactory &&) = delete;      ///< Deleted move assignment operator
    ~PrefabFactory(); ///< Implementation in Prefabfactory.cpp because of std::unique_ptr forward declaration

    ///
    /// \brief Creates a prefabricated entity of type T.
    ///
    /// This method creates a prefabricated entity of type T using the provided registry and tag.
    ///
    /// \param registry The entt::registry to use for creating the prefabricated entity.
    /// \param tag The tag to use for creating the prefabricated entity.
    ///
    template <typename T> std::unique_ptr<Entity> createPrefab(entt::registry &registry, std::string tag)
    {
        static_assert(std::is_base_of_v<Entity, T>, "T must be a subclass of Entity");

        auto entity = std::make_unique<T>(registry, bindlessManager);
        entity->create(device, pipelines.at(PrefabTraits<T>::getPrefabType()).get(), camera, std::move(tag));

        return entity;
    }

    ///
    /// \brief Creates an empty entity that can be customised.
    ///
    /// This method creates an empty entity using the provided registry and tag.
    ///
    /// \param registry The entt::registry to use for creating the custom prefabricated entity.
    /// \param tag The tag to use for creating the custom prefabricated entity.
    ///
    std::unique_ptr<Entity> createCustom(entt::registry &registry, std::string tag);

  private:
    ///
    /// \brief Creates the pipelines for the prefabricated entities.
    ///
    /// This method is used to create the pipelines for the prefabricated entities.
    ///
    void createPipelines();

    const core::Device &device;
    rendering::BindlessManager &bindlessManager;
    const core::RenderPass &renderPass;

    Camera *camera;

    std::unordered_map<PrefabType, std::unique_ptr<core::Pipeline>> pipelines;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_PREFABFACTORY_H
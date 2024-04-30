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

#pragma once

#include "../../common/UUID.h"
#include "../Entity.h"
#include "PrefabTypeManager.h"
#include <entt/entt.hpp>

// Forward declarations
#include "../../core/CoreFwd.h"
#include "../../rendering/RenderingFwd.h"
#include "../../scene/SceneFwd.h"

namespace vkf::scene
{

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
                  const core::RenderPass &renderPass);

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
    template <typename T>
    std::pair<UUID, std::unique_ptr<Prefab>> createPrefab(entt::registry &registry, std::string tag, Scene *scene)
    {
        static_assert(std::is_base_of_v<Prefab, T>, "T must be a subclass of Entity");

        auto prefab = std::make_unique<T>(registry, bindlessManager, Entity{registry});

        std::deque<core::Pipeline *> pipelineDeque;
        for (const auto &pipeline : pipelineMap.at(PrefabTypeManager::getPrefabType<T>()))
        {
            pipelineDeque.emplace_back(pipeline.get());
        }

        auto prefabUUID = prefab->create(device, pipelineDeque, scene, std::move(tag));

        return {prefabUUID, std::move(prefab)};
    }

    [[nodiscard]] PrefabTypeManager::PrefabFunctions getPrefabFunctions(PrefabType type) const;

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

    PrefabTypeManager prefabTypeManager{*this};
    std::unordered_map<PrefabType, std::deque<std::unique_ptr<core::Pipeline>>> pipelineMap;
};

} // namespace vkf::scene
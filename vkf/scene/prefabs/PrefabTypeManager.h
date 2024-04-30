////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PrefabTypeManager.h
/// \brief This file declares the PrefabType enum and the PrefabTraits struct which are used for managing prefabricated
/// entities in a scene.
///
/// The PrefabType enum and the PrefabTraits struct are part of the vkf::scene namespace.
/// They provide utility when managing prefabricated entities in a scene.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <entt/entt.hpp>

// Forward declarations
#include "../../common/UUID.h"
#include "../../core/CoreFwd.h"
#include "../../rendering/RenderingFwd.h"
#include "../../scene/SceneFwd.h"

namespace vkf::scene
{

///
/// \enum PrefabType
/// \brief Enumerates the types of prefabricated entities that can be created.
///
/// This enum provides the types of prefabricated entities that can be created. It contains Cube and Texture2D as the
/// types of prefabricated entities.
///
enum class PrefabType
{
    Cube,
    Texture2D,
    BasemapActor,
    GraticuleActor,
    PoleActor
};

class PrefabTypeManager
{
  public:
    using prefabCreateFunction = std::function<std::pair<UUID, std::unique_ptr<Prefab>>(entt::registry &registry,
                                                                                        std::string tag, Scene *scene)>;
    using pipelineBuildFunction = std::function<std::deque<rendering::PipelineBuilder>(
        const core::Device &device, const core::RenderPass &renderPass, rendering::BindlessManager &bindlessManager)>;

    struct PrefabFunctions
    {
        prefabCreateFunction prefabCreate;
        pipelineBuildFunction pipelineBuild;
    };

    PrefabTypeManager(PrefabFactory &prefabFactory);

    [[nodiscard]] PrefabFunctions getPrefabFunctions(PrefabType type) const;

    ///
    /// \brief Gets the prefab type of a prefabricated entity.
    ///
    /// This method gets the prefab type of a prefabricated entity.
    ///
    /// \tparam T The type of the prefabricated entity.
    /// \return The prefab type of the prefabricated entity.
    ///
    template <typename T> static constexpr PrefabType getPrefabType()
    {
        return PrefabTraits<T>::getPrefabType();
    }

    ///
    /// \brief Gets the prefab name of a prefabricated entity.
    ///
    /// \tparam T The type of the prefabricated entity.
    /// \return The prefab name of the prefabricated entity.
    ///
    template <typename T> static constexpr std::string getPrefabName()
    {
        return PrefabTraits<T>::getPrefabName();
    }

    static std::unordered_map<PrefabType, std::string> prefabNames;

  private:
    std::unordered_map<PrefabType, PrefabFunctions> prefabFunctionsMap;

    ///
    /// \struct PrefabTraits
    /// \brief Provides traits for prefabricated entities.
    ///
    /// This struct provides traits for prefabricated entities. It extends Prefabs with a method to get the PrefabType
    /// of a prefabricated entity.
    ///
    template <typename T> struct PrefabTraits;

#define DEFINE_PREFAB_TRAITS(PrefabClass, prefabType)                                                                  \
    template <> struct PrefabTraits<PrefabClass>                                                                       \
    {                                                                                                                  \
        static constexpr PrefabType getPrefabType()                                                                    \
        {                                                                                                              \
            return prefabType;                                                                                         \
        }                                                                                                              \
        static constexpr std::string getPrefabName()                                                                   \
        {                                                                                                              \
            return #PrefabClass;                                                                                       \
        }                                                                                                              \
    };

    DEFINE_PREFAB_TRAITS(Cube, PrefabType::Cube)
    DEFINE_PREFAB_TRAITS(Texture2D, PrefabType::Texture2D)
    DEFINE_PREFAB_TRAITS(BasemapActor, PrefabType::BasemapActor)
    DEFINE_PREFAB_TRAITS(GraticuleActor, PrefabType::GraticuleActor)
    DEFINE_PREFAB_TRAITS(PoleActor, PrefabType::PoleActor)
};

} // namespace vkf::scene
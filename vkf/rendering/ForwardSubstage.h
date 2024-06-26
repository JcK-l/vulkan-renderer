////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file ForwardSubstage.h
/// \brief This file declares the ForwardSubstage class which is used for managing Vulkan rendering operations.
///
/// The ForwardSubstage class is part of the vkf::rendering namespace. It provides an interface for interacting with
/// Vulkan rendering operations.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RenderSubstage.h"

// Forward declarations
#include "../platform/PlatformFwd.h"
#include "../rendering/RenderingFwd.h"
#include "../scene/SceneFwd.h"

namespace vkf::rendering
{

///
/// \class ForwardSubstage
/// \brief This class manages Vulkan rendering operations.
///
/// It provides an interface for interacting with Vulkan rendering operations. It includes methods for drawing.
///
class ForwardSubstage : public RenderSubstage
{
  public:
    ///
    /// \brief Constructor for the ForwardSubstage class.
    ///
    /// This constructor creates a RenderSubstage using the provided pipeline and GUI.
    ///
    /// \param inputPipeline The Vulkan pipeline.
    /// \param inputGui The GUI.
    ///
    ForwardSubstage(scene::Scene &scene, RenderSource *source, const rendering::BindlessManager &bindlessManager);

    ForwardSubstage(const ForwardSubstage &) = delete;            ///< Deleted copy constructor
    ForwardSubstage(ForwardSubstage &&) noexcept = default;       ///< Default move constructor
    ForwardSubstage &operator=(const ForwardSubstage &) = delete; ///< Deleted copy assignment operator
    ForwardSubstage &operator=(ForwardSubstage &&) = delete;      ///< Deleted move assignment operator
    ~ForwardSubstage() override = default;                        ///< Default destructor

    void draw(vk::raii::CommandBuffer *cmd) override;

    std::string getType() override;

  private:
    scene::Scene &scene;
    RenderSource *source;
    const rendering::BindlessManager &bindlessManager;
};

} // namespace vkf::rendering
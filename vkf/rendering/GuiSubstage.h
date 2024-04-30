////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file GuiSubstage.h
/// \brief This file declares the GuiSubstage class which is used for managing Vulkan rendering operations.
///
/// The GuiSubstage class is part of the vkf::rendering namespace. It provides an interface for interacting with Vulkan
/// rendering operations.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../platform/Gui.h"
#include "RenderSubstage.h"

namespace vkf::rendering
{

///
/// \class GuiSubstage
/// \brief This class manages Vulkan rendering operations.
///
/// It provides an interface for interacting with Vulkan rendering operations. It includes methods for drawing.
///
class GuiSubstage : public RenderSubstage
{
  public:
    ///
    /// \brief Constructor for the GuiSubstage class.
    ///
    /// This constructor creates a RenderSubstage using the provided GUI.
    ///
    /// \param inputGui The GUI.
    ///
    GuiSubstage(platform::Gui *inputGui);

    GuiSubstage(const GuiSubstage &) = delete;            ///< Deleted copy constructor
    GuiSubstage(GuiSubstage &&) noexcept = default;       ///< Default move constructor
    GuiSubstage &operator=(const GuiSubstage &) = delete; ///< Deleted copy assignment operator
    GuiSubstage &operator=(GuiSubstage &&) = delete;      ///< Deleted move assignment operator
    ~GuiSubstage() override = default;                    ///< Default destructor

    void draw(vk::raii::CommandBuffer *cmd) override;

    std::string getType() override;

  private:
    platform::Gui *gui; // The GUI
};

} // namespace vkf::rendering
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file GuiSubstage.cpp
/// \brief This file defines the GuiSubstage class which is used for managing Vulkan rendering operations.
///
/// The GuiSubstage class is part of the vkf::rendering namespace. It provides an interface for interacting with Vulkan
/// rendering operations.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GuiSubstage.h"

namespace vkf::rendering
{

GuiSubstage::GuiSubstage(platform::Gui *inputGui) : gui{inputGui}
{
}

void GuiSubstage::draw(vk::raii::CommandBuffer *cmd)
{
    gui->draw(cmd);
}

std::string GuiSubstage::getType()
{
    return "GUI";
}

} // namespace vkf::rendering
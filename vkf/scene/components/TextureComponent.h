////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file TextureComponent.h
/// \brief This file declares the TextureComponent struct which is used for managing texture data for an entity in a
/// scene.
///
/// The TextureComponent struct is part of the vkf::scene namespace. It provides functionality to create and manage
/// texture data.
///
/// \author Joshua Lowe
/// \date 1/8/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_TEXTURECOMPONENT_H
#define VULKANRENDERER_TEXTURECOMPONENT_H

namespace vkf::core // Forward declarations
{
class Device;
class Image;
} // namespace vkf::core

namespace vkf::scene
{

///
/// \struct TextureComponent
/// \brief Struct for managing texture data for an entity in a scene.
///
/// This struct provides functionality to create and manage texture data.
// It contains a method to create an Image object from a texture file and a string member to store the path to the
// texture file.
///
struct TextureComponent
{
    TextureComponent(const core::Device &device) : device{device} {}; ///< Constructor

    ///
    /// \brief Method to create an Image object from a texture file.
    ///
    /// This method takes a Device object as parameter and creates an Image object from a texture file. The path to the
    /// texture file is stored in the path member. It loads the texture file, creates a staging buffer to transfer the
    /// texture data to the Image object, and transitions the Image object to the appropriate layout for sampling.
    ///
    core::Image createImage();

    void displayGui();

    const core::Device &device;

    std::string path;
    bool hasNewTexture{false};
};

} // namespace vkf::scene

#endif // VULKANRENDERER_TEXTURECOMPONENT_H
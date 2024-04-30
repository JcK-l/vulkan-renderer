////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file TextureComponent.cpp
/// \brief This file implements the TextureComponent struct which is used for managing texture data for an entity in a
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

#include "TextureComponent.h"
#include "../../core/Buffer.h"
#include "../../core/Device.h"
#include "../../core/Image.h"
#include <imgui.h>
#include <stb_image.h>

namespace vkf::scene
{

core::Image TextureComponent::createImage()
{
    device.getHandle().waitIdle();
    this->path = (this->path.empty()) ? PROJECT_ROOT_DIR + std::string("/assets/me.jpg") : this->path;

    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }
    auto texture = core::Image{
        device,
        vk::ImageCreateInfo{.imageType = vk::ImageType::e2D,
                            .format = vk::Format::eR8G8B8A8Srgb,
                            .extent = vk::Extent3D{.width = static_cast<uint32_t>(texWidth),
                                                   .height = static_cast<uint32_t>(texHeight),
                                                   .depth = 1},
                            .mipLevels = 1,
                            .arrayLayers = 1,
                            .samples = vk::SampleCountFlagBits::e1,
                            .tiling = vk::ImageTiling::eOptimal,
                            .usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                            .sharingMode = vk::SharingMode::eExclusive,
                            .initialLayout = vk::ImageLayout::eUndefined},
        VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT};

    vk::BufferCreateInfo bufferCreateInfo{.size = imageSize, .usage = vk::BufferUsageFlagBits::eTransferSrc};
    core::Buffer stagingBuffer{device, bufferCreateInfo,
                               VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                   VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    stagingBuffer.updateData(pixels, imageSize, 0);

    texture.transitionImageLayout(vk::ImageLayout::eTransferDstOptimal);
    texture.copyBufferToImage(stagingBuffer);
    texture.transitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    stbi_image_free(pixels);
    hasNewTexture = false;

    return texture;
}

void TextureComponent::updateGui()
{
    // Maybe use tinyfiledialogs to open a file dialog to select a texture file in the future
    ImGui::Text("Texture:");
    ImGui::Spacing();
    std::string filepathLabel = "File Path##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::InputText(filepathLabel.c_str(), &path[0], path.size());
    if (ImGui::Button("Load"))
    {
        // Load texture
        hasNewTexture = true;
    }
    ImGui::Spacing();
}

} // namespace vkf::scene
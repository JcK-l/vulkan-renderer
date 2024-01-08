////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Gui.h
/// \brief This file declares the Gui class which is used for managing ImGui interfaces.
///
/// The Gui class is part of the vkf::platform namespace. It provides functionality to manage ImGui interfaces,
/// including creating images, image views, and handling ImGui rendering.
///
/// \author Joshua Lowe
/// \date 11/28/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_GUI_H
#define VULKANRENDERER_GUI_H

#include "../core/Image.h"
#include "../rendering/RenderSource.h"
#include "../scene/prefabs/PrefabFactory.h"

// Forward declarations
class ImDrawData;

namespace vkf::core // Forward declarations
{
class Device;
class Instance;
class RenderPass;
class Swapchain;
} // namespace vkf::core

namespace vkf::rendering // Forward declarations
{
class Renderer;
class BindlessManager;
} // namespace vkf::rendering

namespace vkf::scene // Forward declarations
{
class Scene;
class Entity;
} // namespace vkf::scene

namespace vkf::platform
{
class Window;
///
/// \class Gui
/// \brief This class manages ImGui interfaces.
///
/// It provides functionality to manage ImGui interfaces, including creating images, image views, and handling ImGui
/// rendering. Gui is a subclass of RenderSource and therefore provides methods for getting the image views and image
/// count.
///
class Gui : public rendering::RenderSource
{
  public:
    ///
    /// \brief Constructor for the Gui class.
    ///
    /// \param window The window where the Gui will be displayed.
    /// \param instance The Vulkan instance.
    /// \param device The Vulkan device.
    /// \param renderPass The render pass.
    /// \param swapchain The swapchain.
    /// \param scene The scene.
    ///
    Gui(const Window &window, const core::Instance &instance, const core::Device &device,
        const core::RenderPass &renderPass, const core::Swapchain &swapchain,
        rendering::BindlessManager &bindlessManager);

    Gui(const Gui &) = delete;            ///< Deleted copy constructor
    Gui(Gui &&) noexcept = default;       ///< Default move constructor
    Gui &operator=(const Gui &) = delete; ///< Deleted copy assignment operator
    Gui &operator=(Gui &&) = delete;      ///< Deleted move assignment operator
    ~Gui() override;                      ///< Override destructor

    ///
    /// \brief Pre-render operations for the Gui.
    ///
    /// \param renderer The renderer.
    /// \param frameIndex The index of the frame to be rendered.
    ///
    void preRender(scene::Scene &scene);

    void draw(vk::raii::CommandBuffer *cmd);

    [[nodiscard]] std::vector<vk::ImageView> getImageViews() const override;
    [[nodiscard]] uint32_t getImageCount() const override;
    [[nodiscard]] vk::Extent2D getExtent() const override;
    [[nodiscard]] bool resetChanged() override;
    [[nodiscard]] uint32_t getFrameIndex() override;

  private:
    void createImages(uint32_t numImages);
    void createImageViews();

    void createScenePanel(scene::Scene &scene);
    void createHierarchyPanel(scene::Scene &scene);
    void createPropertiesPanel(scene::Scene &scene);

    void createPrefabButtons(scene::Scene &scene);

    const core::Device &device;
    const core::Swapchain &swapchain;
    rendering::BindlessManager &bindlessManager;

    vk::Extent2D sceneViewportExtent{};

    std::vector<core::Image> images;
    std::vector<vk::ImageView> imageViews;

    vk::raii::Sampler textureSampler{VK_NULL_HANDLE};
    vk::raii::DescriptorPool imguiPool{VK_NULL_HANDLE};

    ImDrawData *drawData;

    VkDescriptorSet dset{VK_NULL_HANDLE};

    std::vector<std::pair<scene::PrefabType, std::string>> prefabTypes = {{scene::PrefabType::Cube, "Cube"},
                                                                          {scene::PrefabType::Texture2D, "Texture2D"}};
    uint32_t frameIndex{0};
    bool firstTime{true};
    bool changed{false};
    bool isCreateDialog{false};
};
} // namespace vkf::platform

#endif // VULKANRENDERER_GUI_H
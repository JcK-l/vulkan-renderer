////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Gui.cpp
/// \brief This file implements the Gui class which is used for managing ImGui interfaces.
///
/// The Gui class is part of the vkf::platform namespace. It provides functionality to manage ImGui interfaces,
/// including creating images, image views, and handling ImGui rendering.
///
/// \author Joshua Lowe
/// \date 11/28/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Gui.h"
#include "../common/Log.h"
#include "../core/Device.h"
#include "../core/Instance.h"
#include "../core/PhysicalDevice.h"
#include "../core/RenderPass.h"
#include "../core/Swapchain.h"
#include "../rendering/BindlessManager.h"
#include "../scene/Camera.h"
#include "../scene/Entity.h"
#include "../scene/Scene.h"
#include "Window.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace vkf::platform
{

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

Gui::Gui(const Window &window, const core::Instance &instance, const core::Device &device,
         const core::RenderPass &renderPass, const core::Swapchain &swapchain,
         rendering::BindlessManager &bindlessManager)
    : device{device}, swapchain{swapchain}, bindlessManager{bindlessManager}
{
    vk::SamplerCreateInfo samplerInfo{.magFilter = vk::Filter::eLinear,
                                      .minFilter = vk::Filter::eLinear,
                                      .mipmapMode = vk::SamplerMipmapMode::eLinear,
                                      .addressModeU = vk::SamplerAddressMode::eRepeat,
                                      .addressModeV = vk::SamplerAddressMode::eRepeat,
                                      .addressModeW = vk::SamplerAddressMode::eRepeat,
                                      .anisotropyEnable = VK_TRUE,
                                      .maxAnisotropy = 16,
                                      .compareEnable = VK_FALSE,
                                      .compareOp = vk::CompareOp::eAlways,
                                      .borderColor = vk::BorderColor::eIntOpaqueBlack,
                                      .unnormalizedCoordinates = VK_FALSE};

    textureSampler = device.getHandle().createSampler(samplerInfo);

    //    VkDescriptorPool imguiPool;

    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eSampler, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eSampledImage, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eStorageImage, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eUniformTexelBuffer, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eStorageTexelBuffer, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eUniformBuffer, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eStorageBuffer, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eUniformBufferDynamic, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eStorageBufferDynamic, .descriptorCount = 1000},
        vk::DescriptorPoolSize{.type = vk::DescriptorType::eInputAttachment, .descriptorCount = 1000}};

    vk::DescriptorPoolCreateInfo poolInfo = {.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                             .maxSets = 1000,
                                             .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
                                             .pPoolSizes = poolSizes.data()};

    imguiPool = vk::raii::DescriptorPool{device.getHandle(), poolInfo};

    // this initializes the core structures of imgui
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    //    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    //    ImGui_ImplVulkan_SetMinImageCount(minImageCount);
    ImGui_ImplGlfw_InitForVulkan(window.getHandle(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = *instance.getHandle();
    init_info.PhysicalDevice = *device.getPhysicalDevice().getHandle();
    init_info.Device = *device.getHandle();
    init_info.QueueFamily = device.getQueue(0, 0).getFamilyIndex();
    init_info.Queue = *device.getQueue(0, 0).getHandle();
    init_info.DescriptorPool = *imguiPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = swapchain.getMinImageCount();
    init_info.ImageCount = swapchain.getImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, *renderPass.getHandle());
    LOG_INFO("Created Gui")
}

Gui::~Gui()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::preRender(uint32_t frameIndex, scene::Scene &scene)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool optFullscreen = true;
    static bool optPadding = false;
    static bool dockspaceOpen = true;
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (optFullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        windowFlags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!optPadding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
    if (!optPadding)
        ImGui::PopStyleVar();

    if (optFullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

        if (firstTime)
        {
            firstTime = false;
            activeEntity = std::make_unique<scene::Entity>(scene.getRegistry(), bindlessManager);

            ImGui::DockBuilderRemoveNode(dockspaceId);
            ImGui::DockBuilderAddNode(dockspaceId, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

            ImGuiID dockIdRight;
            ImGuiID dockIdLeft = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.3f, nullptr, &dockIdRight);

            ImGuiID dockIdLeftTop;
            ImGuiID dockIdLeftBottom =
                ImGui::DockBuilderSplitNode(dockIdLeft, ImGuiDir_Down, 0.4f, nullptr, &dockIdLeftTop);

            ImGui::DockBuilderDockWindow("Scene Hierarchy", dockIdLeftTop);
            ImGui::DockBuilderDockWindow("Properties", dockIdLeftBottom);
            ImGui::DockBuilderDockWindow("Scene", dockIdRight);
            ImGui::DockBuilderFinish(dockspaceId);
        }
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &optFullscreen);
            ImGui::MenuItem("Padding", NULL, &optPadding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "",
                                (dockspaceFlags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0))
            {
                dockspaceFlags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
            }
            if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspaceFlags & ImGuiDockNodeFlags_NoDockingSplit) != 0))
            {
                dockspaceFlags ^= ImGuiDockNodeFlags_NoDockingSplit;
            }
            if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspaceFlags & ImGuiDockNodeFlags_NoUndocking) != 0))
            {
                dockspaceFlags ^= ImGuiDockNodeFlags_NoUndocking;
            }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspaceFlags & ImGuiDockNodeFlags_NoResize) != 0))
            {
                dockspaceFlags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspaceFlags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
            {
                dockspaceFlags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "",
                                (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, optFullscreen))
            {
                dockspaceFlags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Close", NULL, false))
                dockspaceOpen = false;
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    createScenePanel(scene, frameIndex);
    createHierarchyPanel(scene);
    createPropertiesPanel();

    ImGui::End();

    ImGui::Render();
}

void Gui::createScenePanel(scene::Scene &scene, uint32_t frameIndex)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Scene");

    if (ImGui::IsWindowHovered())
    {
        ImGuiIO &io = ImGui::GetIO();
        if (io.MouseWheel != 0.0f)
        {
            scene.getCamera()->zoom(-io.MouseWheel);
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f))
        {
            scene.getCamera()->orbit(io.MouseDelta.x, io.MouseDelta.y);
        }
    }

    uint32_t viewportPanelSizeX = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
    uint32_t viewportPanelSizeY = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

    if (viewportPanelSizeX != sceneViewportExtent.width || viewportPanelSizeY != sceneViewportExtent.height)
    {
        LOG_DEBUG("Resizing viewport to {}x{}", viewportPanelSizeX, viewportPanelSizeY)
        sceneViewportExtent.width = viewportPanelSizeX;
        sceneViewportExtent.height = viewportPanelSizeY;
        device.getHandle().waitIdle();
        createImages(swapchain.getImageCount());
        createImageViews();
        ImGui_ImplVulkan_RemoveTexture(dset);
        dset = ImGui_ImplVulkan_AddTexture(*textureSampler, *imageViews[frameIndex],
                                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        changed = true;
        scene.getCamera()->updateAspectRatio(static_cast<float>(sceneViewportExtent.width) /
                                             static_cast<float>(sceneViewportExtent.height));
    }
    ImGui::Image(dset,
                 ImVec2{static_cast<float>(sceneViewportExtent.width), static_cast<float>(sceneViewportExtent.height)});

    ImGui::PopStyleVar();

    // Right-click context menu
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Cube"))
        {
            isCreateDialog = true;
            selectedType = scene::PrefabType::Cube;
        }

        if (ImGui::MenuItem("Create Triangle"))
        {
            isCreateDialog = true;
            selectedType = scene::PrefabType::Triangle;
        }

        ImGui::EndPopup();
    }
    if (isCreateDialog)
        ImGui::OpenPopup("Enter Name");

    static char tag[32] = ""; // Buffer to hold the input text.

    if (ImGui::BeginPopupModal("Enter Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter a name:\n");
        ImGui::Separator();

        // Creates a text input field with the label "Name"
        ImGui::InputText("Name", tag, IM_ARRAYSIZE(tag));

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            // When the OK button is clicked, create the triangle with the name from the input field
            activeEntity = std::move(scene.createEntity(selectedType, tag));
            ImGui::CloseCurrentPopup();
            isCreateDialog = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            isCreateDialog = false;
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void Gui::createPropertiesPanel()
{
    ImGui::Begin("Properties");

    if (activeEntity && activeEntity->getHandle() != entt::null)
    {
        activeEntity->displayGui();
        activeEntity->updateComponents();
    }

    ImGui::End();
}

void Gui::createHierarchyPanel(scene::Scene &scene)
{
    ImGui::Begin("Scene Hierarchy");

    auto view = scene.getRegistry().view<scene::TagComponent>();
    for (auto entity : view)
    {
        auto &tag = view.get<scene::TagComponent>(entity);

        // If the user clicks on the entity name, select this entity
        if (ImGui::Selectable(tag.tag.c_str(), entity == activeEntity->getHandle()))
        {
            activeEntity->setHandle(entity);
        }
    }

    // Right-click context menu
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Destory") && activeEntity->getHandle() != entt::null)
        {
            device.getHandle().waitIdle();
            activeEntity->destroy();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void Gui::draw(vk::raii::CommandBuffer *cmd)
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *(*cmd));

    ImGuiIO &io = ImGui::GetIO();
    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void Gui::createImages(uint32_t numImages)
{
    images.clear();
    images.reserve(numImages);

    for (auto i = 0; i < numImages; ++i)
    {
        vk::ImageCreateInfo imageInfo{
            .imageType = vk::ImageType::e2D,
            .format = vk::Format::eR8G8B8A8Srgb,
            .extent =
                vk::Extent3D{.width = sceneViewportExtent.width, .height = sceneViewportExtent.height, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = vk::SampleCountFlagBits::e1,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined};
        images.emplace_back(device, imageInfo, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    }
}

bool Gui::resetChanged()
{
    bool currentChanged = changed;
    changed = false;
    return currentChanged;
}

vk::Extent2D Gui::getExtent() const
{
    return sceneViewportExtent;
}

std::vector<vk::ImageView> Gui::getImageViews() const
{
    std::vector<vk::ImageView> result;
    result.reserve(imageViews.size());
    for (auto i = 0u; i < images.size(); ++i)
    {
        result.emplace_back(*imageViews[i]);
    }
    return result;
}

uint32_t Gui::getImageCount() const
{
    return swapchain.getImageCount();
}

void Gui::createImageViews()
{
    imageViews.clear();
    imageViews.reserve(images.size());
    for (const auto &image : images)
    {
        imageViews.emplace_back(image.createImageView(vk::ImageAspectFlagBits::eColor));
    }
}

} // namespace vkf::platform
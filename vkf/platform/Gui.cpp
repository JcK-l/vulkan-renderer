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
#include "../common/Utility.h"
#include "../core/Device.h"
#include "../core/Instance.h"
#include "../core/PhysicalDevice.h"
#include "../core/RenderPass.h"
#include "../core/Swapchain.h"
#include "../rendering/BindlessManager.h"
#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "ImGuizmo.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h"
#include <glm/gtc/type_ptr.hpp>

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
    io.Fonts->AddFontFromFileTTF("../../assets/JetBrainsMono/fonts/otf/JetBrainsMono-Regular.otf", 16.0f);
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    //    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    setGuiColorStyle();

    //    ImGui_ImplVulkan_SetMinImageCount(minImageCount);
    ImGui_ImplGlfw_InitForVulkan(window.getHandle(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = *instance.getHandle();
    init_info.PhysicalDevice = *device.getPhysicalDevice().getHandle();
    init_info.Device = *device.getHandle();
    init_info.QueueFamily = device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics).getFamilyIndex();
    init_info.Queue = *device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics).getHandle();
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

void Gui::setGuiColorStyle()
{
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    glm::vec3 primary = calculateLinearColor(glm::vec3{0.173, 0.455, 0.702});
    glm::vec3 secondary = calculateLinearColor(glm::vec3{0.125, 0.325, 0.498});
    glm::vec3 tertiary = calculateLinearColor(glm::vec3{0.549, 0.788, 1.0});
    glm::vec3 black = calculateLinearColor(glm::vec3{0.118, 0.125, 0.133});
    glm::vec3 black75 = calculateLinearColor(glm::vec3{0.337, 0.345, 0.349});
    glm::vec3 black50 = calculateLinearColor(glm::vec3{0.557, 0.561, 0.565});
    glm::vec3 white = calculateLinearColor(glm::vec3{0.988, 0.996, 1.0});
    glm::vec3 white2 = calculateLinearColor(glm::vec3{0.863, 0.937, 1.0});
    glm::vec3 red = calculateLinearColor(glm::vec3{0.702, 0.267, 0.243});
    glm::vec3 red2 = calculateLinearColor(glm::vec3{1.0, 0.667, 0.651});

    // TODO: fix color opactity doing weird things with clear color
    style.Colors[ImGuiCol_Button] = ImVec4(primary.x, primary.y, primary.z, 1);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(secondary.x, secondary.y, secondary.z, 1);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(black.x, black.y, black.z, 0.f);
    style.Colors[ImGuiCol_Header] = ImVec4(primary.x, primary.y, primary.z, 1);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(secondary.x, secondary.y, secondary.z, 1);
    style.Colors[ImGuiCol_Text] = ImVec4(white.x, white.y, white.z, 1);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(black50.x, black50.y, black50.z, 1);
    style.Colors[ImGuiCol_Border] = ImVec4(black75.x, black75.y, black75.z, 1);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(black.x, black.y, black.z, 1);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(white.x, white.y, white.z, 1);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(white2.x, white2.y, white2.z, 1);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(black.x, black.y, black.z, 1);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(black75.x, black75.y, black75.z, 1);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(black50.x, black50.y, black50.z, 1);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(black75.x, black75.y, black75.z, 1);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(black.x, black.y, black.z, 0);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(secondary.x, secondary.y, secondary.z, 1);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(black75.x, black75.y, black75.z, 1);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(black.x, black.y, black.z, 0);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(black.x, black.y, black.z, 1);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(white.x, white.y, white.z, 1);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(white2.x, white2.y, white2.z, 1);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(white2.x, white2.y, white2.z, 1);
    style.Colors[ImGuiCol_Tab] = ImVec4(primary.x, primary.y, primary.z, 1);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_TabActive] = ImVec4(primary.x, primary.y, primary.z, 1);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(secondary.x, secondary.y, secondary.z, 1);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(secondary.x, secondary.y, secondary.z, 1);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(black.x, black.y, black.z, 1);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(white.x, white.y, white.z, 1);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(white2.x, white2.y, white2.z, 1);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(white.x, white.y, white.z, 1);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(white2.x, white2.y, white2.z, 1);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(white.x, white.y, white.z, 1);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(tertiary.x, tertiary.y, tertiary.z, 1);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(black.x, black.y, black.z, 1);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(black.x, black.y, black.z, 1);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(black.x, black.y, black.z, 0);
    style.Colors[ImGuiCol_Separator] = ImVec4(secondary.x, secondary.y, secondary.z, 1);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(black75.x, black75.y, black75.z, 1);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(white2.x, white2.y, white2.z, 1);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(red2.x, red2.y, red2.z, 1);
}

void Gui::preRender(scene::Scene &scene)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    static bool optFullscreen = true;
    static bool optPadding = false;
    static bool dockspaceOpen = true;
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking; // | ImGuiWindowFlags_MenuBar;
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

            ImGui::DockBuilderRemoveNode(dockspaceId);
            ImGui::DockBuilderAddNode(dockspaceId, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

            ImGuiID dockIdRight;
            ImGuiID dockIdLeft = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.35f, nullptr, &dockIdRight);

            ImGuiID dockIdLeftTop;
            ImGuiID dockIdLeftBottom =
                ImGui::DockBuilderSplitNode(dockIdLeft, ImGuiDir_Down, 0.5f, nullptr, &dockIdLeftTop);

            ImGui::DockBuilderDockWindow("Scene Hierarchy", dockIdLeftTop);
            ImGui::DockBuilderDockWindow("Properties", dockIdLeftBottom);
            ImGui::DockBuilderDockWindow("Scene", dockIdRight);
            ImGui::DockBuilderFinish(dockspaceId);
        }
    }

    if (false && ImGui::BeginMenuBar()) // disabled for now
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

    createScenePanel(scene);
    createHierarchyPanel(scene);
    createPropertiesPanel(scene);

    ImGui::End();

    ImGui::Render();
    frameIndex = ++frameIndex % swapchain.getImageCount();
    drawData = ImGui::GetDrawData();
}

void Gui::createScenePanel(scene::Scene &scene)
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

    auto cursorPos = std::make_pair(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y);

    float viewportPanelSizeX = ImGui::GetContentRegionAvail().x;
    float viewportPanelSizeY = ImGui::GetContentRegionAvail().y;

    if (static_cast<uint32_t>(viewportPanelSizeX) != sceneViewportExtent.width ||
        static_cast<uint32_t>(viewportPanelSizeY) != sceneViewportExtent.height)
    {
        LOG_DEBUG("Resizing viewport to {}x{}", viewportPanelSizeX, viewportPanelSizeY)
        sceneViewportExtent.width = static_cast<uint32_t>(viewportPanelSizeX);
        sceneViewportExtent.height = static_cast<uint32_t>(viewportPanelSizeY);
        device.getHandle().waitIdle();
        createImages(swapchain.getImageCount());
        createImageViews();
        ImGui_ImplVulkan_RemoveTexture(dset);
        dset = ImGui_ImplVulkan_AddTexture(*textureSampler, imageViews[frameIndex],
                                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        changed = true;
        scene.getCamera()->updateAspectRatio(viewportPanelSizeX / viewportPanelSizeY);
    }
    ImGui::Image(dset, ImVec2{viewportPanelSizeX, viewportPanelSizeY});

    ImGui::PopStyleVar();

    createGuizmo(scene, cursorPos);

    ImGui::End();
}

void Gui::createGuizmo(scene::Scene &scene, std::pair<float, float> cursorPos)
{
    ImGuiIO &io = ImGui::GetIO();
    if (ImGui::IsKeyPressed(ImGuiKey_G))
    {
        isGizmoEnabled = !isGizmoEnabled;
    }
    if (!isGizmoEnabled)
    {
        return;
    }

    entt::entity guizmoEntity = scene.getActiveEntity();
    if (guizmoEntity == entt::null)
    {
        return;
    }

    auto *relationComp = scene.getRegistry().try_get<scene::RelationComponent>(guizmoEntity);

    if (relationComp != nullptr)
    {
        for (const auto &pair : relationComp->children)
        {
            auto child = pair.second;
            if (scene.getLastSelectedChild() == child->getHandle())
            {
                guizmoEntity = child->getHandle();
            }
        }
    }

    auto *transformComp = scene.getRegistry().try_get<scene::TransformComponent>(guizmoEntity);
    if (transformComp != nullptr)
    {
        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
        ImVec2 windowPos = ImGui::GetWindowPos(); // Get the window's position

        // Calculate the position of the content region relative to the screen
        ImVec2 contentRegionPos = ImVec2(windowPos.x + cursorPos.first, windowPos.y + cursorPos.second);

        //             Now you can use contentRegionPos in the SetRect function
        ImGuizmo::SetRect(contentRegionPos.x, contentRegionPos.y, sceneViewportExtent.width,
                          sceneViewportExtent.height);

        static ImGuizmo::OPERATION currentGizmoOperation;
        if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_T))
            {
                currentGizmoOperation = ImGuizmo::TRANSLATE;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_R))
            {
                currentGizmoOperation = ImGuizmo::ROTATE;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_S))
            {
                currentGizmoOperation = ImGuizmo::SCALE;
            }
        }

        bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
        float snap = 0.25f;
        if (currentGizmoOperation == ImGuizmo::ROTATE)
        {
            snap = 15.f;
        }
        else if (currentGizmoOperation == ImGuizmo::TRANSLATE)
        {
            snap = 10.f;
        }

        float snapValues[3] = {snap, snap, snap};

        ImGuizmo::Manipulate(glm::value_ptr(scene.getCamera()->getViewMatrixFlip()),
                             glm::value_ptr(scene.getCamera()->getProjectionMatrixFlip()), currentGizmoOperation,
                             ImGuizmo::LOCAL, glm::value_ptr(transformComp->modelMatrix), nullptr,
                             useSnap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 scale, translation, rotation;

            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformComp->modelMatrix),
                                                  glm::value_ptr(translation), glm::value_ptr(rotation),
                                                  glm::value_ptr(scale));

            transformComp->translation = translation;
            transformComp->rotation = rotation;
            transformComp->scale = scale;

            transformComp->modelHasChanged = true;
        }
    }
}

void Gui::createHierarchyPanel(scene::Scene &scene)
{
    ImGui::Begin("Scene Hierarchy");

    float windowHeight = ImGui::GetContentRegionAvail().y;

    ImGui::BeginChild("EntityList", ImVec2(0, windowHeight - 45));

    auto view = scene.getRegistry().view<scene::TagComponent, scene::RelationComponent, scene::IdComponent>();
    for (auto entity : view)
    {
        auto &relationComp = view.get<scene::RelationComponent>(entity);
        if (relationComp.parent != entt::null)
        {
            continue;
        }

        auto &tagComp = view.get<scene::TagComponent>(entity);
        auto &uuidComp = view.get<scene::IdComponent>(entity);

        std::string label = tagComp.tag + "##" + std::to_string(static_cast<uint32_t>(entity));

        if (relationComp.hasChildren)
        {
            recursiveHierarchy(scene, uuidComp.uuid, entity);
        }
        else
        {
            createSingleNode(scene, entity, label);

            if (ImGui::IsItemClicked())
            {
                scene.setSeletedPrefab(uuidComp.uuid);
            }
        }
    }

    ImGui::EndChild();

    ImGui::Separator();

    ImGui::BeginChild("PrefabButtons", ImVec2(0, 35));

    createPrefabButtons(scene);

    ImGui::EndChild();

    ImGui::End();
}

void Gui::recursiveHierarchy(scene::Scene &scene, UUID rootUUID, entt::entity entity, bool first)
{
    auto &relationComp = scene.getRegistry().get<scene::RelationComponent>(entity);
    bool isTreeOpen = createParentNode(scene, entity, rootUUID);

    if (ImGui::IsItemClicked() && first)
    {
        auto &uuidComp = scene.getRegistry().get<scene::IdComponent>(entity);
        scene.setSeletedPrefab(uuidComp.uuid);
    }
    else if (ImGui::IsItemClicked() && !first && scene.getLastSelectedChild() == entity)
    {
        scene.setSeletedPrefab(rootUUID);
        scene.setLastSelectedChild(entt::null);
    }
    else if (ImGui::IsItemClicked() && !first)
    {
        scene.setSeletedPrefab(rootUUID);
        scene.setLastSelectedChild(entity);
    }

    if (isTreeOpen)
    {
        for (const auto &pair : relationComp.children)
        {
            auto child = pair.second;
            auto &relationChildComp = child->getComponent<scene::RelationComponent>();
            if (relationChildComp.hasChildren)
            {
                recursiveHierarchy(scene, rootUUID, child->getHandle(), false);
            }
            else
            {
                createChildNode(scene, child->getHandle(), rootUUID);
            }
        }
        ImGui::TreePop();
    }
}

bool Gui::createParentNode(scene::Scene &scene, entt::entity entity, UUID rootUUID)
{
    auto *tagComp = scene.getRegistry().try_get<scene::TagComponent>(entity);

    std::string tag = (tagComp) ? tagComp->tag : "SubMesh";
    std::string label = tag + "##" + std::to_string(static_cast<uint32_t>(entity));

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
    if (entity == scene.getActiveEntity() ||
        (entity == scene.getLastSelectedChild() && rootUUID == scene.getSelectedPrefabUUID()))
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    bool isTreeOpen = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

    return isTreeOpen;
}

void Gui::createChildNode(scene::Scene &scene, entt::entity currentChild, UUID parentUUID)
{
    auto &relationComp = scene.getRegistry().get<scene::RelationComponent>(currentChild);

    ImGuiTreeNodeFlags childnodeFlags =
        ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (scene.getLastSelectedChild() == currentChild && parentUUID == scene.getSelectedPrefabUUID())
    {
        childnodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    auto *meshComp = scene.getRegistry().try_get<scene::MeshComponent>(currentChild);
    auto *tagComp = scene.getRegistry().try_get<scene::TagComponent>(currentChild);

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 text = style.Colors[ImGuiCol_Text];
    ImVec4 textDisabled = style.Colors[ImGuiCol_TextDisabled];

    ImVec4 color = (meshComp && meshComp->shouldDraw) ? text : textDisabled;

    ImGui::PushStyleColor(ImGuiCol_Text, color);

    std::string tag = (tagComp) ? tagComp->tag : "SubMesh";
    std::string label = tag + "##" + std::to_string(static_cast<uint32_t>(currentChild));
    ImGui::TreeNodeEx(label.c_str(), childnodeFlags);

    ImGui::PopStyleColor();

    if (ImGui::IsItemClicked() && scene.getLastSelectedChild() == currentChild)
    {
        scene.setSeletedPrefab(parentUUID);
        scene.setLastSelectedChild(entt::null);
    }
    else if (ImGui::IsItemClicked())
    {
        scene.setSeletedPrefab(parentUUID);
        scene.setLastSelectedChild(currentChild);
    }
}

void Gui::createSingleNode(scene::Scene &scene, entt::entity entity, const std::string &label)
{
    ImGuiTreeNodeFlags nodeFlags =
        ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding;
    if (entity == scene.getActiveEntity())
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    auto *meshComp = scene.getRegistry().try_get<scene::MeshComponent>(entity);

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 text = style.Colors[ImGuiCol_Text];
    ImVec4 textDisabled = style.Colors[ImGuiCol_TextDisabled];

    ImVec4 color = (meshComp && meshComp->shouldDraw) ? text : textDisabled;

    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TreeNodeEx(label.c_str(), nodeFlags);

    ImGui::PopStyleColor();
}

void Gui::createPrefabButtons(scene::Scene &scene)
{
    float windowWidth = ImGui::GetContentRegionAvail().x;
    float windowHeight = ImGui::GetContentRegionAvail().y;
    float spacing = 10.0f;
    float buttonWidth = (windowWidth - spacing) / 2;
    float buttonHeight = 25.0f;
    float padding = windowHeight - buttonHeight;

    ImGui::Dummy(ImVec2(0.0f, padding / 2));

    // Add buttons for creating and destroying entities
    if (ImGui::Button("Create Prefab", ImVec2(buttonWidth, 0)))
    {
        isCreateDialog = true;
    }

    if (isCreateDialog)
        ImGui::OpenPopup("CreatePrefabPopup");

    if (ImGui::BeginPopupContextWindow("CreatePrefabPopup"))
    {

        for (const auto &pair : scene::PrefabTypeManager::prefabNames)
        {
            if (ImGui::MenuItem(pair.second.c_str()))
            {
                scene.createPrefab(pair.first, pair.second);
                ImGui::CloseCurrentPopup();
                isCreateDialog = false;
            }
        }

        // Close the popup when clicking anywhere else
        if (!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(0))
        {
            ImGui::CloseCurrentPopup();
            isCreateDialog = false;
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine(0.0f, spacing);

    if (ImGui::Button("Destroy Prefab", ImVec2(buttonWidth, 0)))
    {
        if (scene.getActiveEntity() != entt::null)
        {
            device.getHandle().waitIdle();
            scene.destroySelectedPrefab();
        }
    }
}

void Gui::createPropertiesPanel(scene::Scene &scene)
{
    ImGui::Begin("Properties");

    if (scene.getActiveEntity() != entt::null)
    {
        scene.updateSelectedPrefabGui();
        scene.updateSelectedPrefabComponents();
        scene.updateGlobalFunctions();
    }

    ImGui::End();
}

void Gui::draw(vk::raii::CommandBuffer *cmd)
{
    ImGui_ImplVulkan_RenderDrawData(drawData, *(*cmd));
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
    return imageViews;
}

uint32_t Gui::getImageCount() const
{
    return swapchain.getImageCount();
}

uint32_t Gui::getFrameIndex()
{
    return (frameIndex - 1 + swapchain.getImageCount()) % swapchain.getImageCount();
}

void Gui::createImageViews()
{
    imageViews.clear();
    imageViews.reserve(images.size());
    for (auto &image : images)
    {
        imageViews.emplace_back(image.getImageView(vk::ImageAspectFlagBits::eColor));
    }
}

} // namespace vkf::platform
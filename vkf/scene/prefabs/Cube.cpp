////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Cube.cpp
/// \brief This file implements the Cube class, which is a type of Entity in the vkf::scene namespace.
///
/// The Cube class is part of the vkf::scene namespace. It provides an interface for creating and managing a Cube
/// entity. A Cube entity in this context is a specific type of Entity that can be used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Cube.h"
#include "../../common/Log.h"
#include "../../core/Shader.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"
#include "../Camera.h"
#include "../Scene.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace vkf::scene
{

Cube::Cube(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
    : Prefab(registry, bindlessManager, std::move(entity))
{
}

UUID Cube::create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene, std::string tag)
{
    entity.create();
    auto prefabUUID = UUID();
    entity.addComponent<scene::IdComponent>(prefabUUID);
    entity.addComponent<scene::TagComponent>(std::move(tag));
    entity.addComponent<scene::ColorComponent>(glm::vec4{1.0f});
    entity.addComponent<scene::RelationComponent>();

    // clang-format off
    std::vector<std::vector<float>> mesh = {{
                                                // Front face
                                                -50, -50, 0, 0.0, 0.0, 1.0,
                                                50, -50, 0, 0.0, 0.0, 1.0,
                                                50, 50, 0, 0.0, 0.0, 1.0,
                                                -50, -50, 0, 0.0, 0.0, 1.0,
                                                50, 50, 0, 0.0, 0.0, 1.0,
                                                -50, 50, 0, 0.0, 0.0, 1.0,
                                            },
                                            {
                                                // Back face
                                                -50, -50, 0, 0.0, 0.0, -1.0,
                                                50, -50, 0, 0.0, 0.0, -1.0,
                                                50, 50, 0, 0.0, 0.0, -1.0,
                                                -50, -50, 0, 0.0, 0.0, -1.0,
                                                50, 50, 0, 0.0, 0.0, -1.0,
                                                -50, 50, 0, 0.0, 0.0, -1.0,
                                            },
                                            {
                                                // Top face
                                                -50, 0, -50, 0.0, 1.0, 0.0,
                                                50, 0, -50, 0.0, 1.0, 0.0,
                                                50, 0, 50, 0.0, 1.0, 0.0,
                                                -50, 0, -50, 0.0, 1.0, 0.0,
                                                50, 0, 50, 0.0, 1.0, 0.0,
                                                -50, 0, 50, 0.0, 1.0, 0.0,
                                            },
                                            {
                                                // Bottom face
                                                -50, 0, -50, 0.0, -1.0, 0.0,
                                                50, 0, -50, 0.0, -1.0, 0.0,
                                                50, 0, 50, 0.0, -1.0, 0.0,
                                                -50, 0, -50, 0.0, -1.0, 0.0,
                                                50, 0, 50, 0.0, -1.0, 0.0,
                                                -50, 0, 50, 0.0, -1.0, 0.0,
                                            },
                                            {
                                                // Right face
                                                0, -50, -50, 1.0, 0.0, 0.0,
                                                0, -50, 50, 1.0, 0.0, 0.0,
                                                0, 50, 50, 1.0, 0.0, 0.0,
                                                0, -50, -50, 1.0, 0.0, 0.0,
                                                0, 50, 50, 1.0, 0.0, 0.0,
                                                0, 50, -50, 1.0, 0.0, 0.0,
                                            },
                                            {
                                                // Left face
                                                0, -50, -50, -1.0, 0.0, 0.0,
                                                0, -50, 50, -1.0, 0.0, 0.0,
                                                0, 50, 50, -1.0, 0.0, 0.0,
                                                0, -50, -50, -1.0, 0.0, 0.0,
                                                0, 50, 50, -1.0, 0.0, 0.0,
                                                0, 50, -50, -1.0, 0.0, 0.0,
                                            }};
    // clang-format on

    // Define transformations for each face
    std::array<glm::vec3, 6> faceTransforms = {
        glm::vec3(0.0f, 0.0f, 50.0f),  // Front face
        glm::vec3(0.0f, 0.0f, -50.0f), // Back face
        glm::vec3(0.0f, 50.0f, 0.0f),  // Top face
        glm::vec3(0.0f, -50.0f, 0.0f), // Bottom face
        glm::vec3(50.0f, 0.0f, 0.0f),  // Right face
        glm::vec3(-50.0f, 0.0f, 0.0f)  // Left face
    };

    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    for (uint32_t i = 0; i < mesh.size(); ++i)
    {
        auto child = Entity(registry);
        child.create();

        child.addComponent<scene::IdComponent>(UUID());
        child.addComponent<scene::TransformComponent>(scene->getCamera(), faceTransforms[i], glm::vec3{0.0f},
                                                      glm::vec3{1.0f});
        child.addComponent<scene::ColorComponent>(glm::vec4{1.0f});
        child.addComponent<scene::RelationComponent>(entity.getHandle());
        auto &meshComp = child.addComponent<MeshComponent>(device);
        meshComp.uploadGeometry(mesh[i], Cube::vertexSize);

        auto &materialComp = child.addComponent<MaterialComponent>(pipelines);

        materialComp.addResource("camera", scene->getCamera()->getHandle());

        vk::BufferCreateInfo bufferCreateInfo{.size = 16, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
        core::Buffer bufferColor{device, bufferCreateInfo,
                                 VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

        vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
        core::Buffer bufferModel{device, bufferModelCreateInfo,
                                 VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

        auto entityBufferHandle = bindlessManager.storeBuffer(bufferColor, vk::BufferUsageFlagBits::eUniformBuffer);
        materialComp.addResource("color", entityBufferHandle);

        auto entityBufferModelHandle =
            bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);
        materialComp.addResource("model", entityBufferModelHandle);
        relationComp.addChild(std::move(child));
    }

    LOG_INFO("Prefab Cube created")
    return prefabUUID;
}

void Cube::updateGui()
{
    auto &tagComp = entity.getComponent<scene::TagComponent>();
    auto &colorComp = entity.getComponent<scene::ColorComponent>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen;
    std::string treeLabel = tagComp.tag + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::TreeNodeEx(treeLabel.c_str(), flags))
    {
        ImGui::Spacing();
        colorComp.updateGui();
        ImGui::Spacing();
    }

    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    if (ImGui::TreeNodeEx("Submeshes", flags))
    {
        ImGui::Spacing();
        for (const auto &pair : relationComp.children)
        {
            auto child = pair.second;
            auto &childColorComp = child->getComponent<scene::ColorComponent>();
            auto &transformComp = child->getComponent<scene::TransformComponent>();
            auto &meshComp = child->getComponent<MeshComponent>();

            transformComp.updateGui();
            childColorComp.updateGui();
            meshComp.updateGui();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }
}

void Cube::updateComponents()
{
    auto &colorComp = entity.getComponent<scene::ColorComponent>();

    bool colorChanged = (this->prevColor != colorComp.color);

    this->prevColor = colorComp.color;

    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    for (const auto &pair : relationComp.children)
    {
        auto child = pair.second;
        auto &transformComp = child->getComponent<scene::TransformComponent>();
        auto &childColorComp = child->getComponent<scene::ColorComponent>();

        if (colorChanged)
        {
            childColorComp.setColor(colorComp.color);
        }

        auto &materialComp = child->getComponent<MaterialComponent>();
        bindlessManager.updateBuffer(materialComp.getResourceIndex("model"), glm::value_ptr(transformComp.modelMatrix),
                                     sizeof(transformComp.modelMatrix), 0);
        bindlessManager.updateBuffer(materialComp.getResourceIndex("color"), glm::value_ptr(childColorComp.color),
                                     sizeof(childColorComp.color), 0);
    }
}

void Cube::destroy()
{
    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    for (const auto &pair : relationComp.children)
    {
        auto child = pair.second;
        auto &materialComp = child->getComponent<MaterialComponent>();
        bindlessManager.removeBuffer(materialComp.getResourceIndex("color"));
        bindlessManager.removeBuffer(materialComp.getResourceIndex("model"));
        child->destroy();
    }

    entity.destroy();
}

uint32_t Cube::vertexSize = 2 * sizeof(glm::vec3);

std::deque<rendering::PipelineBuilder> Cube::getPipelineBuilders(const core::Device &device,
                                                                 const core::RenderPass &renderPass,
                                                                 rendering::BindlessManager &bindlessManager)
{
    auto pipelineBuilder = Prefab::getPipelineBuilder(device, renderPass, bindlessManager);
    core::Shader shader{"../../shaders/cube.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(device, shader);

    vertexSize = 2 * sizeof(glm::vec3);
    auto bindingDescription = vk::VertexInputBindingDescription{
        .binding = 0, .stride = vertexSize, .inputRate = vk::VertexInputRate::eVertex};

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {
        vk::VertexInputAttributeDescription{
            .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = 0},
        vk::VertexInputAttributeDescription{
            .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = sizeof(glm::vec3)}};

    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 1};

    pipelineBuilder.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);

    return {std::move(pipelineBuilder)};
}

} // namespace vkf::scene
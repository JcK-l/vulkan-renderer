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
#include "../components/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace vkf::scene
{

Cube::Cube(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
    : Prefab(registry, bindlessManager, std::move(entity))
{
}

void Cube::create(const core::Device &device, core::Pipeline *pipeline, Camera *camera, std::string tag)
{
    entity.create();
    entity.addComponent<scene::TagComponent>(std::move(tag));
    entity.addComponent<scene::TransformComponent>(glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{1.0f});
    entity.addComponent<scene::ColorComponent>(glm::vec4{1.0f});
    entity.addComponent<scene::PrefabComponent>(PrefabType::Cube);
    entity.addComponent<scene::ParentComponent>();

    // clang-format off
    std::vector<std::vector<float>> mesh = {{
        // Front face
        -0.25, -0.25, 0.25, 0.0, 0.0, 1.0,
        0.25, -0.25, 0.25, 0.0, 0.0, 1.0,
        0.25, 0.25, 0.25, 0.0, 0.0, 1.0,
        -0.25, -0.25, 0.25, 0.0, 0.0, 1.0,
        0.25, 0.25, 0.25, 0.0, 0.0, 1.0,
        -0.25, 0.25, 0.25, 0.0, 0.0, 1.0,
    },
    {
        // Back face
        -0.25, -0.25, -0.25, 0.0, 0.0, -1.0,
        0.25, -0.25, -0.25, 0.0, 0.0, -1.0,
        0.25, 0.25, -0.25, 0.0, 0.0, -1.0,
        -0.25, -0.25, -0.25, 0.0, 0.0, -1.0,
        0.25, 0.25, -0.25, 0.0, 0.0, -1.0,
        -0.25, 0.25, -0.25, 0.0, 0.0, -1.0,
    },
    {
        // Top face
        -0.25, 0.25, -0.25, 0.0, 1.0, 0.0,
        0.25, 0.25, -0.25, 0.0, 1.0, 0.0,
        0.25, 0.25, 0.25, 0.0, 1.0, 0.0,
        -0.25, 0.25, -0.25, 0.0, 1.0, 0.0,
        0.25, 0.25, 0.25, 0.0, 1.0, 0.0,
        -0.25, 0.25, 0.25, 0.0, 1.0, 0.0,
    },
    {
        // Bottom face
        -0.25, -0.25, -0.25, 0.0, -1.0, 0.0,
        0.25, -0.25, -0.25, 0.0, -1.0, 0.0,
        0.25, -0.25, 0.25, 0.0, -1.0, 0.0,
        -0.25, -0.25, -0.25, 0.0, -1.0, 0.0,
        0.25, -0.25, 0.25, 0.0, -1.0, 0.0,
        -0.25, -0.25, 0.25, 0.0, -1.0, 0.0,
    },
    {
        // Right face
        0.25, -0.25, -0.25, 1.0, 0.0, 0.0,
        0.25, -0.25, 0.25, 1.0, 0.0, 0.0,
        0.25, 0.25, 0.25, 1.0, 0.0, 0.0,
        0.25, -0.25, -0.25, 1.0, 0.0, 0.0,
        0.25, 0.25, 0.25, 1.0, 0.0, 0.0,
        0.25, 0.25, -0.25, 1.0, 0.0, 0.0,
    },
    {
        // Left face
        -0.25, -0.25, -0.25, -1.0, 0.0, 0.0,
        -0.25, -0.25, 0.25, -1.0, 0.0, 0.0,
        -0.25, 0.25, 0.25, -1.0, 0.0, 0.0,
        -0.25, -0.25, -0.25, -1.0, 0.0, 0.0,
        -0.25, 0.25, 0.25, -1.0, 0.0, 0.0,
        -0.25, 0.25, -0.25, -1.0, 0.0, 0.0,
    }};
    // clang-format on

    auto &parent = entity.getComponent<scene::ParentComponent>();
    for (auto &subMesh : mesh)
    {
        auto child = Entity(registry);
        child.create();

        child.addComponent<MeshComponent>(device, subMesh);
        child.addComponent<scene::TransformComponent>(glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{1.0f});
        child.addComponent<scene::ColorComponent>(glm::vec4{1.0f});
        child.addComponent<scene::SelectComponent>();

        auto &material = child.addComponent<MaterialComponent>(pipeline);

        material.addUniform("camera", camera->getHandle());

        vk::BufferCreateInfo bufferCreateInfo{.size = 16, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
        core::Buffer bufferColor{device, bufferCreateInfo,
                                 VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

        vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
        core::Buffer bufferModel{device, bufferModelCreateInfo,
                                 VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

        auto entityBufferHandle = bindlessManager.storeBuffer(bufferColor, vk::BufferUsageFlagBits::eUniformBuffer);
        material.addUniform("color", entityBufferHandle);

        auto entityBufferModelHandle =
            bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);
        material.addUniform("model", entityBufferModelHandle);
        parent.addChild(std::move(child));
    }

    LOG_INFO("Prefab Cube created")
}

void Cube::displayGui()
{
    auto &tag = entity.getComponent<scene::TagComponent>();
    auto &transform = entity.getComponent<scene::TransformComponent>();
    auto &color = entity.getComponent<scene::ColorComponent>();

    tag.displayGui();
    transform.displayGui();
    color.displayGui();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Selected Submeshes:");
    ImGui::Separator();
    ImGui::Spacing();

    auto &parent = entity.getComponent<scene::ParentComponent>();
    for (auto child : parent.children)
    {
        auto &select = child->getComponent<scene::SelectComponent>();
        if (!select.selected)
        {
            continue;
        }
        auto &childColor = child->getComponent<scene::ColorComponent>();
        auto &childTransform = child->getComponent<scene::TransformComponent>();
        auto &meshComponent = child->getComponent<MeshComponent>();

        childTransform.displayGui();
        childColor.displayGui();
        meshComponent.displayGui();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
}

void Cube::updateComponents()
{
    auto &transform = entity.getComponent<scene::TransformComponent>();
    auto &color = entity.getComponent<scene::ColorComponent>();

    bool changed = (this->prevColor != color.color);

    this->prevColor = color.color;

    auto model = glm::mat4(1.0f);

    model = glm::translate(model, transform.position);

    model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, transform.scale);

    auto &parent = entity.getComponent<scene::ParentComponent>();
    for (auto child : parent.children)
    {
        auto &childTransform = child->getComponent<scene::TransformComponent>();
        auto &childColor = child->getComponent<scene::ColorComponent>();

        if (changed)
        {
            childColor.setColor(color.color);
        }

        auto childModel = glm::mat4(1.0f);

        childModel = glm::translate(childModel, childTransform.position);

        childModel = glm::rotate(childModel, childTransform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        childModel = glm::rotate(childModel, childTransform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        childModel = glm::rotate(childModel, childTransform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        childModel = glm::scale(childModel, childTransform.scale);

        // Combine the parent and child transformations
        auto combinedModel = model * childModel;

        auto &material = child->getComponent<MaterialComponent>();
        bindlessManager.updateBuffer(material.getUniformIndex("model"), glm::value_ptr(combinedModel),
                                     sizeof(combinedModel), 0);
        bindlessManager.updateBuffer(material.getUniformIndex("color"), glm::value_ptr(childColor.color),
                                     sizeof(childColor.color), 0);
    }
}

void Cube::destroy()
{
    auto &parent = entity.getComponent<scene::ParentComponent>();
    for (auto &child : parent.children)
    {
        auto &material = child->getComponent<MaterialComponent>();
        bindlessManager.removeBuffer(material.getUniformIndex("color"));
        bindlessManager.removeBuffer(material.getUniformIndex("model"));
        child->destroy();
    }

    entity.destroy();
}

Entity &Cube::getEntity()
{
    return entity;
}

void Cube::setEntity(entt::entity ent)
{
    entity.setHandle(ent);
}

rendering::PipelineBuilder Cube::getPipelineBuilder(const core::Device &device, const core::RenderPass &renderPass,
                                                    rendering::BindlessManager &bindlessManager)
{
    auto pipelineBuilder = Prefab::getPipelineBuilder(device, renderPass, bindlessManager);
    core::Shader shader{"../../shaders/cube.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(device, shader);

    auto bindingDescription = vk::VertexInputBindingDescription{
        .binding = 0, .stride = 2 * sizeof(glm::vec3), .inputRate = vk::VertexInputRate::eVertex};

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {
        vk::VertexInputAttributeDescription{
            .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = 0},
        vk::VertexInputAttributeDescription{
            .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = sizeof(glm::vec3)}};

    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 1};

    pipelineBuilder.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);

    return pipelineBuilder;
}

PrefabType Cube::getPrefabType()
{
    return PrefabType::Cube;
}

} // namespace vkf::scene
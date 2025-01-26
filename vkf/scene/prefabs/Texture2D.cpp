////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Texture2D.cpp
/// \brief This file implements the Texture2D class, which is a type of Prefab in the vkf::scene namespace.
///
/// The Texture2D class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// Texture2D prefab. A Texture2D prefab in this context is a specific type of Prefab that can be used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Texture2D.h"
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

Texture2D::Texture2D(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
    : Prefab(registry, bindlessManager, std::move(entity))
{
}

UUID Texture2D::create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene,
                       std::string tag)
{
    entity.create();
    auto prefabUUID = UUID();
    entity.addComponent<scene::IdComponent>(prefabUUID);
    entity.addComponent<scene::TagComponent>(std::move(tag));
    entity.addComponent<scene::TransformComponent>(scene->getCamera(), glm::vec3{0.0f}, glm::vec3{0.0f},
                                                   glm::vec3{1.0f});
    entity.addComponent<scene::RelationComponent>();

    std::vector<float> mesh = {
        // First triangle
        -5.f, 5.f, 0.0f, 0.0f, // Top-left corner
        5.f, 5.f, 1.0f, 0.0f,  // Top-right corner
        5.f, -5.f, 1.0f, 1.0f, // Bottom-right corner

        // Second triangle
        -5.f, 5.f, 0.0f, 0.0f, // Top-left corner
        5.f, -5.f, 1.0f, 1.0f, // Bottom-right corner
        -5.f, -5.f, 0.0f, 1.0f // Bottom-left corner
    };

    auto &meshComp = entity.addComponent<MeshComponent>(device);
    meshComp.uploadGeometry(mesh, Texture2D::vertexSize);

    auto &materialComp = entity.addComponent<MaterialComponent>(std::move(pipelines));

    materialComp.addResource("camera", scene->getCamera()->getHandle());

    vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferModel{device, bufferModelCreateInfo,
                             VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    auto entityBufferModelHandle = bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);
    materialComp.addResource("model", entityBufferModelHandle);

    auto &textureComp = entity.addComponent<scene::TextureComponent>(device);
    auto image = textureComp.createImage();

    auto entityTextureHandle = bindlessManager.storeImage(image);
    materialComp.addResource("texture", entityTextureHandle);

    LOG_INFO("Prefab Texture2D created")
    return prefabUUID;
}

void Texture2D::updateGui()
{
    auto &tagComp = entity.getComponent<scene::TagComponent>();
    auto &transformComp = entity.getComponent<scene::TransformComponent>();
    auto &textureComp = entity.getComponent<scene::TextureComponent>();
    auto &meshComp = entity.getComponent<MeshComponent>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen;
    std::string treeLabel = tagComp.tag + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::TreeNodeEx(treeLabel.c_str(), flags))
    {
        transformComp.updateGui();
        textureComp.updateGui();
        meshComp.updateGui();
    }
}

void Texture2D::updateComponents()
{
    auto &transformComp = entity.getComponent<scene::TransformComponent>();
    auto &textureComp = entity.getComponent<scene::TextureComponent>();

    auto &materialComp = entity.getComponent<MaterialComponent>();
    bindlessManager.updateBuffer(materialComp.getResourceIndex("model"), glm::value_ptr(transformComp.modelMatrix),
                                 sizeof(transformComp.modelMatrix), 0);

    if (textureComp.hasNewTexture)
    {
        auto image = textureComp.createImage();
        bindlessManager.updateImage(materialComp.getResourceIndex("texture"), image);
    }
}

void Texture2D::destroy()
{
    auto &materialComp = entity.getComponent<MaterialComponent>();
    bindlessManager.removeBuffer(materialComp.getResourceIndex("model"));
    bindlessManager.removeImage(materialComp.getResourceIndex("texture"));
    entity.destroy();
}

uint32_t Texture2D::vertexSize = 2 * sizeof(glm::vec2);

std::deque<rendering::PipelineBuilder> Texture2D::getPipelineBuilders(const core::Device &device,
                                                                      const core::RenderPass &renderPass,
                                                                      rendering::BindlessManager &bindlessManager)
{
    auto pipelineBuilder = Prefab::getPipelineBuilder(device, renderPass, bindlessManager);
    core::Shader shader{std::string(PROJECT_ROOT_DIR) + "/shaders/texture2d.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(device, shader);

    auto bindingDescription = vk::VertexInputBindingDescription{
        .binding = 0, .stride = vertexSize, .inputRate = vk::VertexInputRate::eVertex};

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {
        vk::VertexInputAttributeDescription{
            .location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = 0},
        vk::VertexInputAttributeDescription{
            .location = 1, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = sizeof(glm::vec2)}};

    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 1};

    pipelineBuilder.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);

    return {pipelineBuilder};
}

} // namespace vkf::scene
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Texture2D.cpp
/// \brief This file implements the Texture2D class, which is a type of Entity in the vkf::scene namespace.
///
/// The Texture2D class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// Texture2D entity. A Texture2D entity in this context is a specific type of Entity that can be used in a 3D scene.
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
#include "../components/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

Texture2D::Texture2D(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
    : Prefab(registry, bindlessManager, std::move(entity))
{
}

void Texture2D::create(const core::Device &device, core::Pipeline *pipeline, Camera *camera, std::string tag)
{
    entity.create();
    entity.addComponent<scene::TagComponent>(std::move(tag));
    entity.addComponent<scene::TransformComponent>(glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{1.0f});
    entity.addComponent<scene::PrefabComponent>(PrefabType::Texture2D);
    entity.addComponent<scene::ParentComponent>();

    std::vector<float> mesh = {
        // First triangle
        -0.5f, 0.5f, 1.0f, 1.0f, // Top-left corner
        0.5f, 0.5f, 0.0f, 1.0f,  // Top-right corner
        0.5f, -0.5f, 0.0f, 0.0f, // Bottom-right corner

        // Second triangle
        -0.5f, 0.5f, 1.0f, 1.0f, // Top-left corner
        0.5f, -0.5f, 0.0f, 0.0f, // Bottom-right corner
        -0.5f, -0.5f, 1.0f, 0.0f // Bottom-left corner
    };

    entity.addComponent<MeshComponent>(device, mesh);
    auto &material = entity.addComponent<MaterialComponent>(pipeline);

    material.addUniform("camera", camera->getHandle());

    vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferModel{device, bufferModelCreateInfo,
                             VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    auto entityBufferModelHandle = bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);
    material.addUniform("model", entityBufferModelHandle);

    entity.addComponent<scene::TextureComponent>(device);
    auto &texture = entity.getComponent<scene::TextureComponent>();

    auto image = texture.createImage();

    auto entityTextureHandle = bindlessManager.storeImage(image);
    material.addUniform("texture", entityTextureHandle);

    LOG_INFO("Prefab Texture2D created")
}

void Texture2D::displayGui()
{
    auto &tag = entity.getComponent<scene::TagComponent>();
    auto &transform = entity.getComponent<scene::TransformComponent>();
    auto &texture = entity.getComponent<scene::TextureComponent>();
    auto &meshComponent = entity.getComponent<MeshComponent>();

    tag.displayGui();
    transform.displayGui();
    texture.displayGui();
    meshComponent.displayGui();
}

void Texture2D::updateComponents()
{
    auto &transform = entity.getComponent<scene::TransformComponent>();
    auto &texture = entity.getComponent<scene::TextureComponent>();

    auto model = glm::mat4(1.0f);

    model = glm::translate(model, transform.position);

    model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, transform.scale);

    auto &material = entity.getComponent<MaterialComponent>();
    bindlessManager.updateBuffer(material.getUniformIndex("model"), glm::value_ptr(model), sizeof(model), 0);

    if (texture.hasNewTexture)
    {
        auto image = texture.createImage();
        bindlessManager.updateImage(material.getUniformIndex("texture"), image);
    }
}

void Texture2D::destroy()
{
    auto &material = entity.getComponent<MaterialComponent>();
    bindlessManager.removeBuffer(material.getUniformIndex("model"));
    bindlessManager.removeImage(material.getUniformIndex("texture"));
    entity.destroy();
}

Entity &Texture2D::getEntity()
{
    return entity;
}

void Texture2D::setEntity(entt::entity ent)
{
    entity.setHandle(ent);
}

rendering::PipelineBuilder Texture2D::getPipelineBuilder(const core::Device &device, const core::RenderPass &renderPass,
                                                         rendering::BindlessManager &bindlessManager)
{
    auto pipelineBuilder = Prefab::getPipelineBuilder(device, renderPass, bindlessManager);
    core::Shader shader{"../../shaders/texture2d.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(device, shader);

    auto bindingDescription = vk::VertexInputBindingDescription{
        .binding = 0, .stride = 2 * sizeof(glm::vec2), .inputRate = vk::VertexInputRate::eVertex};

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {
        vk::VertexInputAttributeDescription{
            .location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = 0},
        vk::VertexInputAttributeDescription{
            .location = 1, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = sizeof(glm::vec2)}};

    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 1};

    pipelineBuilder.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);

    return pipelineBuilder;
}

PrefabType Texture2D::getPrefabType()
{
    return PrefabType::Texture2D;
}

} // namespace vkf::scene
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Triangle.cpp
/// \brief This file implements the Triangle class, which is a type of Entity in the vkf::scene namespace.
///
/// The Triangle class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// Triangle entity. A Triangle entity in this context is a specific type of Entity that can be used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Triangle.h"
#include "../../common/Log.h"
#include "../../rendering/BindlessManager.h"
#include "../Camera.h"
#include "../components/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

void Triangle::create(const core::Device &device, core::Pipeline *pipeline, Camera *camera, std::string tag)
{
    Entity::create(std::move(tag));

    this->addComponent<scene::TransformComponent>(glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{1.0f});
    this->addComponent<scene::ColorComponent>(glm::vec4{1.0f});

    struct alignas(16) Data
    {
        float data[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    } data;

    vk::BufferCreateInfo bufferCreateInfo{.size = sizeof(data), .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer buffer{device, bufferCreateInfo,
                        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferModel{device, bufferModelCreateInfo,
                             VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    this->addComponent<scene::MaterialComponent>(pipeline);
    auto &entityMaterialComponent = this->getComponent<scene::MaterialComponent>();
    entityMaterialComponent.addBuffer("camera", camera->getHandle());

    auto entityBufferHandle = bindlessManager.storeBuffer(buffer, vk::BufferUsageFlagBits::eUniformBuffer);
    entityMaterialComponent.addBuffer("color", entityBufferHandle);

    auto entityBufferModelHandle = bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);
    entityMaterialComponent.addBuffer("model", entityBufferModelHandle);
    LOG_INFO("Prefab Triangle created")
}

void Triangle::displayGui()
{
    Entity::displayGui();

    auto &transform = this->getComponent<scene::TransformComponent>();
    auto &color = this->getComponent<scene::ColorComponent>();

    transform.displayGui();
    color.displayGui();
}

void Triangle::updateComponents()
{
    Entity::updateComponents();

    auto &material = this->getComponent<scene::MaterialComponent>();
    auto &transform = this->getComponent<scene::TransformComponent>();
    auto &color = this->getComponent<scene::ColorComponent>();

    auto model = glm::mat4(1.0f);

    model = glm::translate(model, transform.position);

    model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, transform.scale);

    bindlessManager.updateBuffer(material.getBufferIndex("model"), glm::value_ptr(model), sizeof(model), 0);
    bindlessManager.updateBuffer(material.getBufferIndex("color"), glm::value_ptr(color.color), sizeof(color.color), 0);
}

void Triangle::destroy()
{
    auto &material = this->getComponent<scene::MaterialComponent>();
    bindlessManager.removeBuffer(material.getBufferIndex("color"));
    bindlessManager.removeBuffer(material.getBufferIndex("model"));

    Entity::destroy();
}

} // namespace vkf::scene
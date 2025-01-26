////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PoleActor.cpp
/// \brief This file implements the PoleActor class, which is a type of Prefab in the vkf::scene namespace.
///
/// The PoleActor class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// PoleActor Prefab. A PoleActor Prefab in this context is a specific type of Prefab that can be used in a 3D
/// scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PoleActor.h"
#include "../../common/Log.h"
#include "../../core/Device.h"
#include "../../core/Shader.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"
#include "../Camera.h"
#include "../Scene.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace vkf::scene
{

PoleActor::PoleActor(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
    : Prefab(registry, bindlessManager, std::move(entity))
{
}

UUID PoleActor::create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene,
                       std::string tag)
{
    this->device = &device;
    this->scene = scene;
    this->pipelines = std::move(pipelines);

    entity.create();
    auto prefabUUID = UUID();
    entity.addComponent<scene::IdComponent>(prefabUUID);
    entity.addComponent<scene::TagComponent>(std::move(tag));
    entity.addComponent<scene::ColorComponent>(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

    entity.addComponent<scene::RelationComponent>();

    LOG_INFO("Prefab GraticuleActor created")
    return prefabUUID;
}

void PoleActor::updateGui()
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

        float windowWidth = ImGui::GetContentRegionAvail().x;
        float spacing = 10.0f;
        float buttonWidth = (windowWidth - spacing) / 2;

        if (ImGui::Button("Create Pole", ImVec2(buttonWidth, 0)))
        {
            createPole();
        }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::Button("Destroy Pole", ImVec2(buttonWidth, 0)))
        {
            destroyPole();
        }

        ImGui::Spacing();
    }

    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    if (ImGui::TreeNodeEx("Submeshes", flags))
    {
        ImGui::Spacing();
        for (const auto &pair : relationComp.children)
        {
            auto pole = pair.second;
            auto &relationPoleComp = pole->getComponent<scene::RelationComponent>();
            auto &transformComp = pole->getComponent<scene::TransformComponent>();
            auto &poleComp = pole->getComponent<scene::PoleComponent>();

            transformComp.updateGui();
            poleComp.updateGui();

            for (int i{}; const auto &childPair : relationPoleComp.children)
            {
                auto child = childPair.second;
                auto &meshComp = child->getComponent<scene::MeshComponent>();

                if (poleComp.hasChanged)
                {
                    auto &materialComp = child->getComponent<MaterialComponent>();

                    int pipelineIndex = (poleComp.isTube) ? static_cast<int>(i) + 2 : static_cast<int>(i);
                    materialComp.setPipeline(pipelineIndex);
                }

                meshComp.updateGui();
                i++;
            }
            poleComp.hasChanged = false;

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }
}

void PoleActor::updateComponents()
{
    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    auto &colorComp = entity.getComponent<scene::ColorComponent>();

    bool colorChanged = (this->prevColor != colorComp.color);

    this->prevColor = colorComp.color;

    for (const auto &pair : relationComp.children)
    {
        auto pole = pair.second;
        auto &relationPoleComp = pole->getComponent<scene::RelationComponent>();
        auto &transformComp = pole->getComponent<scene::TransformComponent>();
        auto &poleComp = pole->getComponent<PoleComponent>();

        if (colorChanged)
        {
            poleComp.poleData.geometryColor = colorComp.color;
        }

        for (const auto &childPair : relationPoleComp.children)
        {
            auto child = childPair.second;
            auto &materialComp = child->getComponent<MaterialComponent>();

            bindlessManager.updateBuffer(materialComp.getResourceIndex("model"),
                                         glm::value_ptr(transformComp.modelMatrix), sizeof(transformComp.modelMatrix),
                                         0);
            bindlessManager.updateBuffer(materialComp.getResourceIndex("data"), &poleComp.poleData,
                                         sizeof(poleComp.poleData), 0);
        }
    }
}

void PoleActor::destroy()
{
    auto &relationComp = entity.getComponent<scene::RelationComponent>();

    for (auto &pair : relationComp.children)
    {
        auto pole = pair.second;
        auto &relationPoleComp = pole->getComponent<scene::RelationComponent>();
        auto &idComp = pole->getComponent<scene::IdComponent>();

        for (bool once{true}; auto &childPair : relationPoleComp.children)
        {
            auto child = childPair.second;
            auto &materialComp = child->getComponent<MaterialComponent>();
            if (once)
            {
                bindlessManager.removeBuffer(materialComp.getResourceIndex("data"));
                bindlessManager.removeBuffer(materialComp.getResourceIndex("model"));
                once = false;
            }
            child->destroy();
        }

        scene->removeGlobalFunction(idComp.uuid);
        pole->destroy();
    }
    entity.destroy();
}

void PoleActor::createPole()
{
    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    auto pole = Entity(registry);
    pole.create();

    auto &idComp = pole.addComponent<scene::IdComponent>(UUID());
    pole.addComponent<scene::TagComponent>("Pole");
    pole.addComponent<scene::TransformComponent>(scene->getCamera(), glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{1.0f});
    auto &poleComp = pole.addComponent<scene::PoleComponent>(scene->getCamera());

    vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferModel{*device, bufferModelCreateInfo,
                             VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    auto entityBufferModelHandle = bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);

    vk::BufferCreateInfo bufferDataCreateInfo{.size = sizeof(PoleComponent::PoleData),
                                              .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferData{*device, bufferDataCreateInfo,
                            VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    auto entityBufferDataHandle = bindlessManager.storeBuffer(bufferData, vk::BufferUsageFlagBits::eUniformBuffer);

    scene->addGlobalFunction(idComp.uuid, [&, entityBufferDataHandle]() {
        poleComp.updateData();
        bindlessManager.updateBuffer(entityBufferDataHandle, &poleComp.poleData, sizeof(poleComp.poleData), 0);
    });

    auto &relationPoleComp = pole.addComponent<scene::RelationComponent>(entity.getHandle());

    auto poleLines = Entity(registry);
    auto poleTicks = Entity(registry);
    std::array<Entity, 2> children{std::move(poleLines), std::move(poleTicks)};

    for (int i{}; auto &child : children)
    {
        child.create();
        child.addComponent<scene::IdComponent>(UUID());
        child.addComponent<scene::TagComponent>((i == 0) ? "Pole Lines" : "Pole Ticks");
        child.addComponent<scene::RelationComponent>(pole.getHandle());
        auto &materialComp = child.addComponent<MaterialComponent>(pipelines);

        materialComp.setPipeline(i);

        materialComp.addResource("camera", scene->getCamera()->getHandle());

        materialComp.addResource("model", entityBufferModelHandle);

        materialComp.addResource("data", entityBufferDataHandle);

        auto &meshComp = child.addComponent<scene::MeshComponent>(*device);

        uploadGeometry(static_cast<PoleType>(i), meshComp);

        relationPoleComp.addChild(std::move(child));
        i++;
    }

    relationComp.addChild(std::move(pole));
}

void PoleActor::destroyPole()
{
    auto &relationComp = entity.getComponent<scene::RelationComponent>();

    for (auto &pair : relationComp.children)
    {
        auto pole = pair.second;
        auto &relationPoleComp = pole->getComponent<scene::RelationComponent>();
        auto &idComp = pole->getComponent<scene::IdComponent>();

        if (scene->getLastSelectedChild() != pole->getHandle())
        {
            continue;
        }

        device->getHandle().waitIdle();

        for (bool once{true}; auto &childPair : relationPoleComp.children)
        {
            auto child = childPair.second;
            auto &materialComp = child->getComponent<MaterialComponent>();
            if (once)
            {
                bindlessManager.removeBuffer(materialComp.getResourceIndex("data"));
                bindlessManager.removeBuffer(materialComp.getResourceIndex("model"));
                once = false;
            }
            child->destroy();
        }

        scene->removeGlobalFunction(idComp.uuid);
        relationComp.removeChild(idComp.uuid);
        pole->destroy();
        break;
    }
}

void PoleActor::uploadGeometry(PoleType type, MeshComponent &meshComponent)
{
    auto vertices = std::vector<float>{};
    double pTop = 100.0;
    double pBottom = 1050.0;

    switch (type)
    {
    case PoleType::Line: {
        vertices.reserve(2 * 3);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(static_cast<float>(pBottom));
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(static_cast<float>(pTop));
        meshComponent.uploadGeometry(vertices, vertexSize);
        break;
    }
    case PoleType::Tick: {
        int interval = 100;
        int p = int(pBottom / interval) * interval;
        while (p >= pTop)
        {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(static_cast<float>(p));
            p -= interval;
        }

        meshComponent.uploadGeometry(vertices, vertexSize);
        break;
    }
    }
}

uint32_t PoleActor::vertexSize = sizeof(glm::vec3);

std::deque<rendering::PipelineBuilder> PoleActor::getPipelineBuilders(const core::Device &device,
                                                                      const core::RenderPass &renderPass,
                                                                      rendering::BindlessManager &bindlessManager)
{
    auto pipelineBuilders = std::deque<rendering::PipelineBuilder>{};

    // create PipelineBuilder for pole lines
    auto pipelineBuilderPoleLines = Prefab::getPipelineBuilder(device, renderPass, bindlessManager);

    vertexSize = sizeof(glm::vec3);
    auto bindingDescription = vk::VertexInputBindingDescription{
        .binding = 0, .stride = vertexSize, .inputRate = vk::VertexInputRate::eVertex};

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {vk::VertexInputAttributeDescription{
        .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = 0}};

    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 1};

    pipelineBuilderPoleLines.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);

    pipelineBuilderPoleLines.setInputAssemblyCreateInfo(
        vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::eLineList});

    pipelineBuilderPoleLines.setRasterizerCreateInfo(vk::PipelineRasterizationStateCreateInfo{
        .polygonMode = vk::PolygonMode::eFill, .frontFace = vk::FrontFace::eCounterClockwise, .lineWidth = 3.0f});

    core::Shader shaderPoleLines{std::string(PROJECT_ROOT_DIR) + "/shaders/pole_lines.glsl"};
    pipelineBuilderPoleLines.setShaderStageCreateInfos(device, shaderPoleLines);

    // create PipelineBuilder for pole tubes
    auto pipelineBuilderPoleTubes = rendering::PipelineBuilder(pipelineBuilderPoleLines);

    core::Shader shaderPoleTubes{std::string(PROJECT_ROOT_DIR) + "/shaders/pole_tubes.glsl"};
    pipelineBuilderPoleTubes.setShaderStageCreateInfos(device, shaderPoleTubes);

    // create PipelineBuilder for tick lines
    auto pipelineBuilderTickLines = rendering::PipelineBuilder(pipelineBuilderPoleLines);

    pipelineBuilderTickLines.setInputAssemblyCreateInfo(
        vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::ePointList});

    core::Shader shaderTickLines{std::string(PROJECT_ROOT_DIR) + "/shaders/tick_lines.glsl"};
    pipelineBuilderTickLines.setShaderStageCreateInfos(device, shaderTickLines);

    // create PipelineBuilder for tick tubes
    auto pipelineBuilderTickTubes = rendering::PipelineBuilder(pipelineBuilderTickLines);

    core::Shader shaderTickTubes{std::string(PROJECT_ROOT_DIR) + "/shaders/tick_tubes.glsl"};
    pipelineBuilderTickTubes.setShaderStageCreateInfos(device, shaderTickTubes);

    // add all PipelineBuilders to the deque
    pipelineBuilders.push_back(pipelineBuilderPoleLines);
    pipelineBuilders.push_back(pipelineBuilderTickLines);

    pipelineBuilders.push_back(pipelineBuilderPoleTubes);
    pipelineBuilders.push_back(pipelineBuilderTickTubes);

    return pipelineBuilders;
}

} // namespace vkf::scene
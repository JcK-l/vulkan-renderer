////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file BasemapActor.cpp
/// \brief This file implements the BasemapActor class, which is a type of Prefab in the vkf::scene namespace.
///
/// The BasemapActor class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// BasemapActor prefab. A BasemapActor prefab in this context is a specific type of Prefab that can be used in a 3D
/// scene.
///
/// \author Joshua Lowe
/// \date 1/9/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BasemapActor.h"
#include "../../common/Log.h"
#include "../../core/Shader.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"
#include "../Camera.h"
#include "../Scene.h"
#include <imgui.h>

namespace vkf::scene
{

BasemapActor::BasemapActor(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
    : Prefab(registry, bindlessManager, std::move(entity))
{
}

UUID BasemapActor::create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene,
                          std::string tag)
{
    entity.create();
    auto prefabUUID = UUID();
    entity.addComponent<scene::IdComponent>(prefabUUID);
    entity.addComponent<scene::TagComponent>(std::move(tag));
    entity.addComponent<scene::RelationComponent>();
    entity.addComponent<scene::BoundingBoxComponent>();
    auto &projectionComp = entity.addComponent<scene::ProjectionComponent>();
    projectionComp.projectionFlags = ProjectionFlags::CYLINDRICAL_ENABLE | ProjectionFlags::ROTATEDLATLON_ENABLE;

    // this is x and z
    std::vector<float> mesh = {
        -180.0f, 90.0f,  // llcrnrlon, urcrnrlat
        180.0f,  90.0f,  // urcrnrlon, urcrnrlat
        -180.0f, -90.0f, // llcrnrlon, llcrnrlat
        180.0f,  -90.0f, // urcrnrlon, llcrnrlat
    };

    auto &meshComp = entity.addComponent<MeshComponent>(device);
    meshComp.uploadGeometry(mesh, BasemapActor::vertexSize);

    auto &materialComp = entity.addComponent<MaterialComponent>(std::move(pipelines));

    materialComp.addResource("camera", scene->getCamera()->getHandle());

    vk::BufferCreateInfo bufferModelCreateInfo{.size = sizeof(GeotiffComponent::Data),
                                               .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferData{device, bufferModelCreateInfo,
                            VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    auto entityBufferModelHandle = bindlessManager.storeBuffer(bufferData, vk::BufferUsageFlagBits::eUniformBuffer);
    materialComp.addResource("data", entityBufferModelHandle);

    auto &geotiffComp = entity.addComponent<scene::GeotiffComponent>(device);
    auto image = geotiffComp.createImage();

    auto entityTextureHandle = bindlessManager.storeImage(image);
    materialComp.addResource("texture", entityTextureHandle);

    LOG_INFO("Prefab BasemapActor created")
    return prefabUUID;
}

void BasemapActor::updateGui()
{
    auto &tagComp = entity.getComponent<scene::TagComponent>();
    auto &geotiffComp = entity.getComponent<scene::GeotiffComponent>();
    auto &projectionComp = entity.getComponent<scene::ProjectionComponent>();
    auto &bboxComp = entity.getComponent<scene::BoundingBoxComponent>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen;
    std::string treeLabel = tagComp.tag + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::TreeNodeEx(treeLabel.c_str(), flags))
    {
        bboxComp.updateGui();
        projectionComp.updateGui();
        geotiffComp.updateGui();
    }
}

void BasemapActor::updateComponents()
{
    auto &materialComp = entity.getComponent<MaterialComponent>();
    auto &geotiffComp = entity.getComponent<GeotiffComponent>();
    auto &projectionComp = entity.getComponent<ProjectionComponent>();
    auto &bboxComp = entity.getComponent<BoundingBoxComponent>();

    geotiffComp.data.poleLat = projectionComp.rotatedNorthPoleLatitude;
    geotiffComp.data.poleLon = projectionComp.rotatedNorthPoleLongitude;

    bindlessManager.updateBuffer(materialComp.getResourceIndex("data"), &geotiffComp.data, sizeof(geotiffComp.data), 0);

    if (geotiffComp.hasNewTexture)
    {
        auto image = geotiffComp.createImage();
        bindlessManager.updateImage(materialComp.getResourceIndex("texture"), image);
    }

    if (bboxComp.hasNewBbox)
    {
        auto &meshComp = entity.getComponent<MeshComponent>();
        meshComp.uploadGeometry(bboxComp.getMeshData(), BasemapActor::vertexSize);
        bboxComp.hasNewBbox = false;
    }

    switch (projectionComp.mapProjection)
    {
    case ProjectionType::CYLINDRICAL:
        materialComp.setPipeline(0);
        break;
    case ProjectionType::ROTATEDLATLON:
        materialComp.setPipeline(1);
        break;
    case ProjectionType::PROJ_LIBRARY:
        // Not enabled so this is not needed
        break;
    }
}

void BasemapActor::destroy()
{
    auto &materialComp = entity.getComponent<MaterialComponent>();
    bindlessManager.removeBuffer(materialComp.getResourceIndex("data"));
    bindlessManager.removeImage(materialComp.getResourceIndex("texture"));
    entity.destroy();
}

uint32_t BasemapActor::vertexSize = sizeof(glm::vec2);

std::deque<rendering::PipelineBuilder> BasemapActor::getPipelineBuilders(const core::Device &device,
                                                                         const core::RenderPass &renderPass,
                                                                         rendering::BindlessManager &bindlessManager)
{
    auto pipelineBuilders = std::deque<rendering::PipelineBuilder>{};

    auto pipelineBuilder = Prefab::getPipelineBuilder(device, renderPass, bindlessManager);

    pipelineBuilder.setInputAssemblyCreateInfo(
        vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::eTriangleStrip});

    core::Shader shader{"../../shaders/basemap.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(device, shader);

    auto bindingDescription = vk::VertexInputBindingDescription{
        .binding = 0, .stride = vertexSize, .inputRate = vk::VertexInputRate::eVertex};

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {vk::VertexInputAttributeDescription{
        .location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = 0}};

    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 1};

    pipelineBuilder.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);

    auto pipelineBuilder2 = rendering::PipelineBuilder(pipelineBuilder);

    core::Shader shader2{"../../shaders/basemap_rotated.glsl"};
    pipelineBuilder2.setShaderStageCreateInfos(device, shader2);

    pipelineBuilders.push_back(pipelineBuilder);
    pipelineBuilders.push_back(pipelineBuilder2);

    return pipelineBuilders;
}

} // namespace vkf::scene
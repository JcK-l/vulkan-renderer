////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file GraticuleActor.cpp
/// \brief This file implements the GraticuleActor class, which is a type of Prefab in the vkf::scene namespace.
///
/// The GraticuleActor class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// GraticuleActor Prefab. A GraticuleActor Prefab in this context is a specific type of Prefab that can be used in a 3D
/// scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GraticuleActor.h"
#include "../../common/Log.h"
#include "../../common/Utility.h"
#include "../../core/Shader.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"
#include "../Camera.h"
#include "../Scene.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace vkf::scene
{

GraticuleActor::GraticuleActor(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
    : Prefab(registry, bindlessManager, std::move(entity))
{
}

UUID GraticuleActor::create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene,
                            std::string tag)
{
    entity.create();
    auto prefabUUID = UUID();
    entity.addComponent<scene::IdComponent>(prefabUUID);
    entity.addComponent<scene::TagComponent>(std::move(tag));
    entity.addComponent<scene::ColorComponent>(glm::vec4{1.0f});
    entity.addComponent<scene::TransformComponent>(scene->getCamera(), glm::vec3{0.0f}, glm::vec3{0.0f},
                                                   glm::vec3{1.0f});

    GDALAllRegister();
    entity.addComponent<scene::GraticuleComponent>();
    entity.addComponent<scene::RelationComponent>();
    entity.addComponent<scene::ProjectionComponent>();
    auto &bboxComp = entity.addComponent<scene::BoundingBoxComponent>();
    bboxComp.isInput = true;

    vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferModel{device, bufferModelCreateInfo,
                             VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    entityBufferModelHandle = bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);

    std::array<std::string, 3> names = {"Graticule", "Coastline", "Borderline"};
    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    for (uint32_t i = 0; i < 3; ++i)
    {
        auto child = Entity(registry);
        child.create();

        child.addComponent<scene::IdComponent>(UUID());
        child.addComponent<scene::TagComponent>(std::move(names[i]));
        child.addComponent<scene::ColorComponent>(glm::vec4{1.0f});
        child.addComponent<scene::RelationComponent>(entity.getHandle());

        auto &meshComp = child.addComponent<scene::MeshComponent>(device);
        meshComp.multiDraw = true;
        uploadGeometry(static_cast<GraticuleType>(i), meshComp);

        auto &materialComp = child.addComponent<MaterialComponent>(pipelines);

        materialComp.addResource("camera", scene->getCamera()->getHandle());

        vk::BufferCreateInfo bufferCreateInfo{.size = 16, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
        core::Buffer bufferColor{device, bufferCreateInfo,
                                 VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

        auto entityBufferHandle = bindlessManager.storeBuffer(bufferColor, vk::BufferUsageFlagBits::eUniformBuffer);
        materialComp.addResource("color", entityBufferHandle);

        materialComp.addResource("model", entityBufferModelHandle);
        relationComp.addChild(std::move(child));
    }

    LOG_INFO("Prefab GraticuleActor created")
    return prefabUUID;
}

void GraticuleActor::updateGui()
{
    auto &tagComp = entity.getComponent<scene::TagComponent>();
    auto &colorComp = entity.getComponent<scene::ColorComponent>();
    auto &transformComp = entity.getComponent<scene::TransformComponent>();
    auto &graticuleComp = entity.getComponent<scene::GraticuleComponent>();
    auto &projectionComp = entity.getComponent<scene::ProjectionComponent>();
    auto &bboxComp = entity.getComponent<scene::BoundingBoxComponent>();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen;
    std::string treeLabel = tagComp.tag + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::TreeNodeEx(treeLabel.c_str(), flags))
    {
        ImGui::Spacing();
        colorComp.updateGui();
        transformComp.updateGui();
        bboxComp.updateGui();
        projectionComp.updateGui();
        graticuleComp.updateGui();
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
            auto &meshComp = child->getComponent<scene::MeshComponent>();
            auto &childTagComp = child->getComponent<scene::TagComponent>();

            childTagComp.updateGui();
            childColorComp.updateGui();
            meshComp.updateGui();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }
}

void GraticuleActor::updateComponents()
{
    auto &transformComp = entity.getComponent<scene::TransformComponent>();
    auto &graticuleComp = entity.getComponent<scene::GraticuleComponent>();
    uint32_t graticule{};
    auto &bboxComp = entity.getComponent<scene::BoundingBoxComponent>();

    if (bboxComp.hasNewBbox)
    {
        graticuleComp.hasNewGraticule = true;
        bbox = bboxComp.getRectF();
        bboxComp.hasNewBbox = false;
    }

    auto &colorComp = entity.getComponent<scene::ColorComponent>();
    bool colorChanged = (this->prevColor != colorComp.color);

    this->prevColor = colorComp.color;

    auto &relationComp = entity.getComponent<scene::RelationComponent>();
    for (const auto &pair : relationComp.children)
    {
        auto child = pair.second;
        if (graticuleComp.hasNewGraticule)
        {
            auto &meshComp = child->getComponent<scene::MeshComponent>();
            uploadGeometry(static_cast<GraticuleType>(graticule++), meshComp);
        }

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

    if (graticuleComp.hasNewGraticule)
    {
        graticuleComp.hasNewGraticule = false;
    }
}

void GraticuleActor::destroy()
{
    auto &relationComp = entity.getComponent<scene::RelationComponent>();

    bindlessManager.removeBuffer(entityBufferModelHandle);
    for (const auto &pair : relationComp.children)
    {
        auto child = pair.second;
        auto &materialComp = child->getComponent<MaterialComponent>();
        bindlessManager.removeBuffer(materialComp.getResourceIndex("color"));
        child->destroy();
    }
    entity.destroy();
}

void GraticuleActor::uploadGeometry(GraticuleType type, MeshComponent &meshComp)
{
    LOG_DEBUG("Generating graticule and coast-/borderline geometry...")

    auto &graticuleComp = entity.getComponent<scene::GraticuleComponent>();
    auto &projectionComp = entity.getComponent<scene::ProjectionComponent>();

    // Instantiate utility class for geometry handling.
    Met3D::GeometryHandling geo;
    geo.initProjProjection(projectionComp.projLibraryString);

    auto rotatedNorthPole =
        Met3D::PointF(projectionComp.rotatedNorthPoleLongitude, projectionComp.rotatedNorthPoleLatitude);
    geo.initRotatedLonLatProjection(rotatedNorthPole);

    // Heuristic value to eliminate line segments that after projection cross
    // the map domain due to a connection that after projection is invalid.
    // This happens when a line segment that connects two closeby vertices after
    // projection leaves e.g. the eastern side of the map and re-enters on the
    // western side (or vice versa).
    // A value of "20deg" seems to work well with global data from NaturalEarth.
    // NOTE (mr, 28Oct2020): This is "quick&dirty" workaround. The correct
    // approach to this would be to perform some sort of test that checks if
    // the correct connection of the two vertices after projection should cross
    // map boundaries, in such as case the segment needs to be broken up.
    // Another approach to this was previously implemented in BT's code, see
    // Met.3D version 1.6 or earlier.
    double rotatedGridMaxSegmentLength_deg = 20.;

    // Get bounding box in which the graticule will be displayed.
    auto geometryLimits = Met3D::RectF(-180., -90., 180., 90.);

    std::vector<float> geometry;

    switch (type)
    {
    case GraticuleType::Graticule: {
        // Generate graticule geometry.
        // ============================
        std::vector<float> graticuleLongitudes =
            createRange(graticuleComp.graticuleLongitudes[0], graticuleComp.graticuleLongitudes[1],
                        graticuleComp.graticuleLongitudes[2]);
        std::vector<float> graticuleLatitudes =
            createRange(graticuleComp.graticuleLatitudes[0], graticuleComp.graticuleLatitudes[1],
                        graticuleComp.graticuleLatitudes[2]);

        Met3D::Vector2D graticuleSpacing = Met3D::Vector2D(
            Met3D::PointF(graticuleComp.graticuleSpacingLongitude, graticuleComp.graticuleSpacingLatitude));

        // Generate graticule geometry.
        std::vector<std::vector<Met3D::PointF>> graticule =
            geo.generate2DGraticuleGeometry(graticuleLongitudes, graticuleLatitudes, graticuleSpacing);

        // Project and clip the generated graticule geometry.
        graticule = projectAndClipGeometry(&geo, graticule, bbox, rotatedGridMaxSegmentLength_deg);

        // Convert list of polygons to vertex list for OpenGL rendering.
        std::vector<float> verticesGraticule;
        geo.flattenPolygonsToVertexList(graticule, &verticesGraticule, &meshComp.startIndices, &meshComp.vertexCounts);
        LOG_DEBUG("Graticule and coast-/borderline geometry was generated.")
        geometry = verticesGraticule;
        break;
    }
    case GraticuleType::Coastline: {
        // Read coastline geometry from shapefile.
        // =======================================

        // For projection and clippling to work correctly, we load coastline and
        // borderline geometry on the entire globe, then clip to the bounding
        // box after projection. Performance seems to be accaptable (mr, 28Oct2020).

        std::string coastFile = PROJECT_ROOT_DIR + std::string("/assets/ne_50m_coastline/ne_50m_coastline.shp");
        std::vector<std::vector<Met3D::PointF>> coastlines = geo.read2DGeometryFromShapefile(coastFile, geometryLimits);

        coastlines = projectAndClipGeometry(&geo, coastlines, bbox, rotatedGridMaxSegmentLength_deg);

        std::vector<float> verticesCoastlines;
        geo.flattenPolygonsToVertexList(coastlines, &verticesCoastlines, &meshComp.startIndices,
                                        &meshComp.vertexCounts);
        LOG_DEBUG("Graticule and coast-/borderline geometry was generated.")
        geometry = verticesCoastlines;
        break;
    }
    case GraticuleType::Borderline: {
        // Read borderline geometry from shapefile.
        // ========================================

        std::string borderFile =
            PROJECT_ROOT_DIR +
            std::string("/assets/ne_50m_admin_0_boundary_lines_land/ne_50m_admin_0_boundary_lines_land.shp");
        std::vector<std::vector<Met3D::PointF>> borderlines =
            geo.read2DGeometryFromShapefile(borderFile, geometryLimits);

        borderlines = projectAndClipGeometry(&geo, borderlines, bbox, rotatedGridMaxSegmentLength_deg);

        std::vector<float> verticesBorderlines;
        geo.flattenPolygonsToVertexList(borderlines, &verticesBorderlines, &meshComp.startIndices,
                                        &meshComp.vertexCounts);
        LOG_DEBUG("Graticule and coast-/borderline geometry was generated.")
        geometry = verticesBorderlines;
        break;
    }
    default:
        LOG_ERROR("Graticule type not supported.")
        return;
    }

    meshComp.uploadGeometry(geometry, vertexSize);
}

std::vector<std::vector<Met3D::PointF>> GraticuleActor::projectAndClipGeometry(
    Met3D::GeometryHandling *geo, std::vector<std::vector<Met3D::PointF>> geometry, Met3D::RectF bbox,
    double rotatedGridMaxSegmentLength_deg)
{
    auto &projectionComp = entity.getComponent<scene::ProjectionComponent>();

    // Projection-dependent operations.
    if (projectionComp.mapProjection == ProjectionType::CYLINDRICAL)
    {
        // Cylindrical projections may display bounding boxed outside the
        // -180..180 degrees range, hence enlarge the geometry if required.
        // TODO: Joshua: This doesn't work correctly yet with bigger bounding boxes.
        geometry = geo->enlargeGeometryToBBoxIfNecessary(geometry, bbox);
    }
    else if (projectionComp.mapProjection == ProjectionType::PROJ_LIBRARY)
    {
        geometry = geo->geographicalToProjectedCoordinates(geometry);
    }
    else if (projectionComp.mapProjection == ProjectionType::ROTATEDLATLON)
    {
        geometry = geo->geographicalToRotatedCoordinates(geometry);
        geometry = geo->splitLineSegmentsLongerThanThreshold(geometry, rotatedGridMaxSegmentLength_deg);
    }

    // Clip line geometry to the bounding box that is rendered.
    geometry = geo->clipPolygons(geometry, bbox);

    return geometry;
}

uint32_t GraticuleActor::vertexSize = sizeof(glm::vec2);

std::deque<rendering::PipelineBuilder> GraticuleActor::getPipelineBuilders(const core::Device &device,
                                                                           const core::RenderPass &renderPass,
                                                                           rendering::BindlessManager &bindlessManager)
{
    auto pipelineBuilder = Prefab::getPipelineBuilder(device, renderPass, bindlessManager);

    pipelineBuilder.setInputAssemblyCreateInfo(
        vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::eLineStrip});

    core::Shader shader{std::string(PROJECT_ROOT_DIR) + "/shaders/simple_geometry.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(device, shader);

    pipelineBuilder.setRasterizerCreateInfo(vk::PipelineRasterizationStateCreateInfo{
        .polygonMode = vk::PolygonMode::eFill, .frontFace = vk::FrontFace::eCounterClockwise, .lineWidth = 2.0f});

    vertexSize = sizeof(glm::vec2);
    auto bindingDescription = vk::VertexInputBindingDescription{
        .binding = 0, .stride = vertexSize, .inputRate = vk::VertexInputRate::eVertex};

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {vk::VertexInputAttributeDescription{
        .location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = 0}};

    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 1};

    pipelineBuilder.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);

    return {std::move(pipelineBuilder)};
}

} // namespace vkf::scene
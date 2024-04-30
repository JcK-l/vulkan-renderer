/// \file
/// \brief

//
// Created by Joshua Lowe on 1/28/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace vkf::scene
{

// Forward declarations
class Camera;

struct PoleComponent
{

    struct PoleData
    {
        glm::vec4 geometryColor{0.0f, 0.0f, 0.0f, 1.0f};
        glm::vec2 pToWorldZParams{std::log(1050.0f), (36.0f - 0.0f) / (log(20.0f) - log(1050.0f))};
        alignas(16) glm::vec3 cameraPosition{};
        alignas(16) glm::vec3 offsetDirection{};
        float tubeRadius{0.1f};
        float endSegmentOffset{0.1f};
    };

    explicit PoleComponent(scene::Camera *camera);

    void updateGui();

    void updateData();

    Camera *camera{nullptr};

    PoleData poleData;
    bool isTube{false};

    bool hasChanged{false};
};

} // namespace vkf::scene
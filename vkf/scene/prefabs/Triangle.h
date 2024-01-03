////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Triangle.h
/// \brief This file declares the Triangle class, which is a type of Entity in the vkf::scene namespace.
///
/// The Triangle class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// Triangle entity. A Triangle entity in this context is a specific type of Entity that can be used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_TRIANGLE_H
#define VULKANRENDERER_TRIANGLE_H

#include "../Entity.h"

namespace vkf::core // Forward declarations
{
class Device;
class Pipeline;
} // namespace vkf::core

namespace vkf::scene
{

// Forward declarations
class Camera;

///
/// \class Triangle
/// \brief This class manages Triangle entities.
///
/// This class provides an interface for creating and managing a Triangle entity. A Triangle entity in this context is a
/// specific type of Entity that can be used in a 3D scene.
///
class Triangle : public Entity
{
  public:
    ///
    /// \brief Constructs a Triangle object.
    ///
    /// This constructor initializes the registry reference with the provided registry, by passing it to the Base class.
    ///
    /// \param registry Reference to the entt::registry
    ///
    explicit Triangle(entt::registry &registry, rendering::BindlessManager &bindlessManager)
        : Entity(registry, bindlessManager){};

    ///
    /// \brief Creates a Triangle entity.
    ///
    /// This method creates a Triangle entity using the provided device, bindlessManager, pipeline, camera, and tag.
    ///
    /// \param device The Vulkan device to use for creating the Triangle entity.
    /// \param bindlessManager The BindlessManager to use for creating the Triangle entity.
    /// \param pipeline The Pipeline to use for creating the Triangle entity.
    /// \param camera The Camera to use for creating the Triangle entity.
    /// \param tag The tag to use for creating the Triangle entity.
    ///
    void create(const core::Device &device, core::Pipeline *pipeline, Camera *camera, std::string tag);

    void destroy() override;

    ///
    /// \brief Displays the GUI for the Triangle entity.
    ///
    /// This method is overridden from the Entity base class and is used to display the GUI for the Triangle entity.
    ///
    void displayGui() override;

    ///
    /// \brief Updates the components of the Triangle entity.
    ///
    /// This method is overridden from the Entity base class and is used to update the components of the Triangle
    /// entity.
    ///
    /// \param bindlessManager The BindlessManager to use for updating the components of the Triangle entity.
    ///
    void updateComponents() override;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_TRIANGLE_H
/// \file
/// \brief

//
// Created by Joshua Lowe on 12/18/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_ENTITY_H
#define VULKANRENDERER_ENTITY_H

#include <entt/entt.hpp>

namespace vkf::scene
{

class Entity
{
  public:
    explicit Entity(entt::registry &registry);
    ~Entity() = default;

    template <typename T, typename... Args> void addComponent(Args &&...args)
    {
        assert(handle != entt::null && "Entity is not valid");
        registry.emplace<T>(handle, std::forward<Args>(args)...);
    }

    template <typename T> T &getComponent()
    {
        assert(handle != entt::null && "Entity is not valid");
        return registry.get<T>(handle);
    }

    void create();
    void destroy();

    [[nodiscard]] entt::entity getHandle() const;
    void setHandle(entt::entity handle);
    void setHandle(Entity entity);

  private:
    entt::registry &registry;
    entt::entity handle{entt::null};
};

} // namespace vkf::scene

#endif // VULKANRENDERER_ENTITY_H
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "application/Application.h"
#include "common/logging.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

int main() {

  vkf::logging::init();
  vkf::Application app{"VulkanRenderer"};
  app.run();

  return 0;
}

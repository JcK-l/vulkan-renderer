//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

#include "vk_mem_alloc.h"


import vkf.platform.Application;
import vkf.common.log;


int main() {

  vkf::common::log::init();
  vkf::platform::Application app{"VulkanRenderer"};
  app.run();

  return 0;
}

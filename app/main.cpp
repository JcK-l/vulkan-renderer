#include "../vkf/platform/application.h"


int main() {

//  vkf::common::log::init();
  vkf::platform::Application app{"VulkanRenderer"};
  app.run();

  return 0;
}

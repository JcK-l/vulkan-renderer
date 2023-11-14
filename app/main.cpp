#include "../vkf/platform/application.h"


int main() {

  vkf::platform::Application::initLogger();
  vkf::platform::Application app{"VulkanRenderer"};
  app.run();

  return 0;
}

## November 6 - First Commit

This is the first update report. I've written a couple of classes that make up the base of this project.
From here on out I'll start writing more vulkan code. Next up is device and surface creation.

In the following sections I'll talk about some of the things that I've implemented.

### Application

This class contains the main loop of the program, which at the moment doesn't include much.

```c
void Application::run() {
    while (!window->isClosed()) {
        onUpdate();
        window->onUpdate();
        // drawFrame();
    }
}
```

It currently calls two update functions. The window onUpdate function just calls glfwPollEvents and the
application onUpdate isn't implemented yet.

It also features an onEvent function which receives events from the
GLFWwindow. This function is getting passed as a callback function to the window class.

```c
void Application::onEvent(Event &event) {
  switch (event.type) {
    case Event::Type::Keyboard: {
      auto data = std::get<Event::Keyboard>(event.data);
      LOG_INFO("KeyboardEvent: keycode={}, action={}", data.keycode, data.action);
      break;
    }
    case Event::Type::MouseMove: {
      auto data = std::get<Event::MouseMove>(event.data);
      LOG_INFO("MouseMoveEvent: x={}, y={}", data.xPos, data.yPos);
      break;
    }
    case Event::Type::MouseButton: {
      auto data = std::get<Event::MouseButton>(event.data);
      LOG_INFO("MouseButtonEvent: button={}, action={}", data.button, data.action);
      break;
    }
    case Event::Type::MouseScroll: {
      auto data = std::get<Event::MouseScroll>(event.data);
      LOG_INFO("MouseScrollEvent: x={}, y={}", data.xScroll, data.yScroll);
      break;
    }
    case Event::Type::Resize: {
      auto data = std::get<Event::Resize>(event.data);
      LOG_INFO("ResizeEvent: width={}, height={}", data.newWidth, data.newHeight);
      break;
    }
    case Event::Type::Close: {
      LOG_INFO("CloseEvent");
      break;
    }
  }
}
```

This simple system will probably work fine for everything we need. In the future we might need to forward the events to
the gui, but that can just be a method call from a gui class.

### Window

The window class receives a properties struct in its constructor.

```c
struct Properties {
    std::string title;
    Mode mode{Mode::Windowed};
    bool resizeable{false};
    bool resized{false};
    Vsync vsync{Vsync::ON};
    Extent extent{800, 600};
    std::function<void(Event &)> eventCallback;
};
```

These settings are then used to initialize glfw. Because we're only going to use vulkan (and not directX for example),
there is no need to abstract the window class for different operating systems. Also, we are using glfw which
is cross-platform already.

This is also where the eventCallback gets called. This is one example.

```c
// ...

glfwSetWindowSizeCallback(handle,[](GLFWwindow *window, int width, int height) {
if (auto *data = reinterpret_cast<Properties *>(glfwGetWindowUserPointer(window))) {
data->extent.width = width;
data->extent.height = height;
data->resized = true;
Event event{ Event::Type::Resize, Event::Resize{ width, height }};
data->eventCallback(event);
}
});

// ...
```

It uses glfwGetWindowUserPointer to get the windowData and then uses the callback which gets set in the application.

### Events

Events are currently just a struct with a type and data.

```c
struct Event {
    // more structs...

    struct Resize {
        int newWidth;
        int newHeight;
    };

    struct Close {
    };

    enum class Type {
        Keyboard, MouseMove, MouseButton, MouseScroll, Resize, Close
    };
    using Data = std::variant<Keyboard, MouseMove, MouseButton, MouseScroll, Resize, Close>;

    Type type;
    Data data;
};
```

The type can then be checked in switch statement and the data can be accessed.

### Input Polling

Sometimes it can be helpful to poll input. Glfw provides functions to do just that.
To make those functions more easily accessible the glfw functions are wrapped in a namespace called input.
As the need for more functionality arises, more functions will be added.

```c
namespace vkf::input {
    bool isKeyPressed(GLFWwindow *window, int keycode);

    bool isMouseButtonPressed(GLFWwindow *window, int button);

    std::pair<double, double> getMousePosition(GLFWwindow *window);

    double getMouseX(GLFWwindow *window);

    double getMouseY(GLFWwindow *window);
} // vkf::input
```

### Logging

This project uses [spdlog](https://github.com/gabime/spdlog). The init function gets called in the main entry point of
the program. The macros
can then be called to log relevant information.

```c
#define LOG_INFO(...) spdlog::info(__VA_ARGS__);
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__);
#define LOG_ERROR(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__);

namespace vkf::logging {
    void init();
} // namespace vkf::logging
```

### Vulkan Hpp

This project uses [vulkan.hpp](https://github.com/KhronosGroup/Vulkan-Hpp).
This means the code will look a bit different from the standard Vulkan API. Because this project is compiled using the
c++20 standard, I also use designated initializers. In order to use them you need to #define VULKAN_HPP_NO_CONSTRUCTORS.
This makes the code look cleaner in my opinion. Here an example from the vulkan hpp gitHub.

```c
// initialize the vk::ApplicationInfo structure
vk::ApplicationInfo applicationInfo{
.pApplicationName   = AppName,
.applicationVersion = 1,
.pEngineName        = EngineName,
.engineVersion      = 1,
.apiVersion         = VK_API_VERSION_1_1
};

// initialize the vk::InstanceCreateInfo
vk::InstanceCreateInfo instanceCreateInfo{ .pApplicationInfo = & applicationInfo };
```

#### Dynamic Loader

I'm using the Dynamic loader that vulkan-hpp provides. In order to use it, a couple of macros have to be defined.

```c
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_CONSTRUCTORS

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
```

To create it there are three steps:

```c
vk::DynamicLoader dl;
auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

// ...

vk::Instance instance = vk::createInstance({}, nullptr);
VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

// ...

std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
assert(!physicalDevices.empty());
vk::Device device = physicalDevices[0].createDevice({}, nullptr);
VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
```

There is a detailed explanation in the [vulkan.hpp](https://github.com/KhronosGroup/Vulkan-Hpp) gitHub README.md.

#### Raii

With raii you don't have to worry about cleaning up your vulkan handles. Here an example from the gitHub.

```c
// create a vk::raii::Device, given a vk::raii::PhysicalDevice physicalDevice and a vk::DeviceCreateInfo deviceCreateInfo
vk::raii::Device device( physicalDevice, deviceCreateInfo );
```

This device gets destroyed automatically when it leaves its scope. There are other benefits, but also some
inconveniences.
All of this can be found in
the [raii programing guide](https://github.com/KhronosGroup/Vulkan-Hpp/blob/main/vk_raii_ProgrammingGuide.mdk).

### Vulkan Instance and Debug Messenger

Instance is the first core class that I've implemented. It tries to enable all the extensions that were passed as
required extensions in the constructor and sets up the debug messenger.

The debug messenger only gets initialized if in debug mode.

```c
#if !defined( NDEBUG )
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             VkDebugUtilsMessengerCallbackDataEXT const *callbackData,
                                             void * /*pUserData*/ ) {
  // Log debug message
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    LOG_WARN("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    LOG_ERROR("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
  }
  return VK_FALSE;
}

#endif
```

```c
#if !defined( NDEBUG )
    debugMessenger = vk::raii::DebugUtilsMessengerEXT{handle,
                                                      common::utils::createDebugMessengerInfo(&debugCallback)};
#endif
```

```c
#if !defined( NDEBUG )
    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instanceCreateInfo = {
    vk::InstanceCreateInfo{
      .pApplicationInfo = &applicationInfo,
      .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
      .ppEnabledLayerNames = enabledLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
      .ppEnabledExtensionNames = enabledExtensions.data()},
      common::utils::createDebugMessengerInfo(&debugCallback)
    };
#else
    vk::StructureChain<vk::InstanceCreateInfo> instanceCreateInfo = {
    vk::InstanceCreateInfo{
      .pApplicationInfo = &applicationInfo},
    };
#endif
```

In order to track issues that can arise during instance creation or destruction, we pass the debugUtilsCreateInfo to the
pNext extensions field of
instanceCreateInfo. [extension documentation](https://github.com/KhronosGroup/Vulkan-Docs/blob/main/appendices/VK_EXT_debug_utils.adoc#examples)

For this we're using vulkan-hpp structure chains. During instance creation we can get the creatInfo by simply calling

```c
handle = vk::raii::Instance{context, instanceCreateInfo.get<vk::InstanceCreateInfo>()};
```

### Results

When starting the program, this is the terminal output:

![not showing image](../updates/November_6_images/terminal.png)

The logger gets initialized and the layers/extensions get enabled.

![not showing image](../updates/November_6_images/window.png)

The Window doesn't display anything yet but works like any other window.

![not showing image](../updates/November_6_images/terminal2.png)

Events get outputted in the terminal at the moment, but will be used to interact with the application.

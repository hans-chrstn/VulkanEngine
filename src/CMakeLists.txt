set(ENGINE_SOURCES
  main.cpp
  core/window.cpp
  renderer/context.cpp
  renderer/instance.cpp
  renderer/logical_device.cpp
  renderer/physical_device.cpp
  renderer/surface.cpp
  renderer/swapchain.cpp
  renderer/image_views.cpp
  math/engine_math.cpp
)

add_executable(VulkanTest ${ENGINE_SOURCES})
target_include_directories(VulkanTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VulkanTest PRIVATE Vulkan::Vulkan glfw)

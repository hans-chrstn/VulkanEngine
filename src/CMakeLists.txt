set(ENGINE_SOURCES
  main.cpp
  core/window.cpp
  core/input.cpp
  renderer/context.cpp
  renderer/instance.cpp
  renderer/logical_device.cpp
  renderer/physical_device.cpp
  renderer/surface.cpp
  renderer/swapchain.cpp
  renderer/image_views.cpp
  renderer/pipeline.cpp
  renderer/command_pool.cpp
  renderer/command_buffers.cpp
  renderer/sync_objects.cpp
  renderer/buffer.cpp
  renderer/stb_image_impl.cpp
  renderer/texture.cpp
  renderer/camera.cpp
  renderer/font.cpp
  ui/context.cpp
  ui/layout_manager.cpp
  math/engine_math.cpp
)

add_executable(VulkanTest ${ENGINE_SOURCES})
target_include_directories(VulkanTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Freetype REQUIRED)
target_include_directories(VulkanTest PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(VulkanTest PRIVATE Vulkan::Vulkan glfw ${FREETYPE_LIBRARIES})

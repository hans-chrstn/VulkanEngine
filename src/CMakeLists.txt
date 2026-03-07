set(ENGINE_SOURCES
  main.cpp
  core/logger.cpp
  core/logger.hpp
  core/window.cpp
  core/window.hpp
  math/engine_math.cpp
  math/engine_math.hpp
)

add_executable(VulkanTest ${ENGINE_SOURCES})
target_include_directories(VulkanTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VulkanTest PRIVATE Vulkan::Vulkan glfw)

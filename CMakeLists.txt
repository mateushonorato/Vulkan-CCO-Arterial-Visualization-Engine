cmake_minimum_required(VERSION 3.16)
project(VulkanCCOEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_SHARED_LIBS "Build shared libs" OFF)
option(ENABLE_WARNINGS "Enable extra compiler warnings" ON)

if(ENABLE_WARNINGS)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
  endif()
endif()

# Find Vulkan (robust): try CMake, then pkg-config, then VULKAN_SDK env
find_package(Vulkan QUIET)
if (NOT Vulkan_FOUND)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_VULKAN QUIET vulkan)
    if (PC_VULKAN_FOUND)
      add_library(Vulkan::Vulkan INTERFACE)
      target_include_directories(Vulkan::Vulkan INTERFACE ${PC_VULKAN_INCLUDE_DIRS})
      target_link_directories(Vulkan::Vulkan INTERFACE ${PC_VULKAN_LIBRARY_DIRS})
      target_link_libraries(Vulkan::Vulkan INTERFACE ${PC_VULKAN_LIBRARIES})
      set(Vulkan_FOUND TRUE)
    endif()
  endif()
endif()
if (NOT Vulkan_FOUND)
  if (DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK $ENV{VULKAN_SDK})
    # Try typical paths
    if (EXISTS "${VULKAN_SDK}/include/vulkan/vulkan.h")
      add_library(Vulkan::Vulkan INTERFACE)
      target_include_directories(Vulkan::Vulkan INTERFACE "${VULKAN_SDK}/include")
      # Link library if present
      if (EXISTS "${VULKAN_SDK}/lib/libvulkan.so")
        target_link_libraries(Vulkan::Vulkan INTERFACE "${VULKAN_SDK}/lib/libvulkan.so")
      elseif (EXISTS "${VULKAN_SDK}/lib64/libvulkan.so")
        target_link_libraries(Vulkan::Vulkan INTERFACE "${VULKAN_SDK}/lib64/libvulkan.so")
      endif()
      set(Vulkan_FOUND TRUE)
    endif()
  endif()
endif()
if (NOT Vulkan_FOUND)
  message(FATAL_ERROR "Vulkan not found. Install Vulkan SDK or system Vulkan dev package (e.g., `sudo apt install libvulkan-dev vulkan-tools`).")
endif()

# Find GLFW (prefer package config, fallback to find_package)
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(GLFW3 QUIET glfw3)
endif()
find_package(glfw3 QUIET)

# Find GLM
find_package(glm QUIET)

# Sources
set(SRC
  src/main.cpp
  src/app.cpp
  src/vtk_reader.cpp
)

add_executable(VulkanCCOEngine ${SRC})

# Include directories
target_include_directories(VulkanCCOEngine PRIVATE include)

# Link Vulkan
target_link_libraries(VulkanCCOEngine PRIVATE Vulkan::Vulkan)

# Link GLFW if found
if (TARGET glfw)
  target_link_libraries(VulkanCCOEngine PRIVATE glfw)
elseif (glfw3_FOUND)
  target_link_libraries(VulkanCCOEngine PRIVATE ${glfw3_LIBRARIES})
  target_include_directories(VulkanCCOEngine PRIVATE ${glfw3_INCLUDE_DIRS})
elseif (GLFW3_FOUND)
  target_link_libraries(VulkanCCOEngine PRIVATE ${GLFW3_LIBRARIES})
  target_include_directories(VulkanCCOEngine PRIVATE ${GLFW3_INCLUDE_DIRS})
else()
  message(WARNING "GLFW not found. Window creation will not work until installed.")
endif()

# Link GLM if found
if (glm_FOUND)
  target_link_libraries(VulkanCCOEngine PRIVATE glm)
else()
  message(STATUS "GLM not found; assuming header-only usage if available.")
endif()

# Provide source data directory to the app
target_compile_definitions(VulkanCCOEngine PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")

# Shader compile helper (requires glslc)
find_program(GLSLC_EXECUTABLE NAMES glslc HINTS ENV VULKAN_SDK PATH_SUFFIXES bin)
if (GLSLC_EXECUTABLE)
  file(GLOB SHADER_SOURCES "${CMAKE_SOURCE_DIR}/shaders/*.glsl")
  set(SPIRV_OUTPUTS)
  foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    set(OUT_SPV "${CMAKE_SOURCE_DIR}/shaders/${SHADER_NAME}.spv")
    # Infer stage from filename
    set(SHADER_STAGE "")
    if (SHADER_NAME STREQUAL "vert")
      set(SHADER_STAGE "-fshader-stage=vertex")
    elseif (SHADER_NAME STREQUAL "frag")
      set(SHADER_STAGE "-fshader-stage=fragment")
    elseif (SHADER_NAME MATCHES ".*\.vert")
      set(SHADER_STAGE "-fshader-stage=vertex")
    elseif (SHADER_NAME MATCHES ".*\.frag")
      set(SHADER_STAGE "-fshader-stage=fragment")
    endif()
    add_custom_command(
      OUTPUT ${OUT_SPV}
      COMMAND ${GLSLC_EXECUTABLE} ${SHADER_STAGE} -o ${OUT_SPV} ${SHADER}
      DEPENDS ${SHADER}
      COMMENT "Compiling ${SHADER_NAME}.glsl to SPIR-V"
      VERBATIM
    )
    list(APPEND SPIRV_OUTPUTS ${OUT_SPV})
  endforeach()
  add_custom_target(Shaders ALL DEPENDS ${SPIRV_OUTPUTS})
else()
  message(WARNING "glslc not found; shaders won't auto-compile. Install Vulkan SDK.")
endif()

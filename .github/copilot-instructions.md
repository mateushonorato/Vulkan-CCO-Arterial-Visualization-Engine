# Copilot Instructions for Vulkan-CCO-Arterial-Visualization-Engine

These instructions guide AI coding agents working in this repository. Focus on the project’s concrete structure, build workflow, and conventions to be immediately productive.

## Project Overview
- Purpose: Interactive visualization of CCO arterial tree models, evolving from 2D (TP1) to 3D (TP2).
- Tech stack: C++17, Vulkan, GLFW (window), GLM (math), GLSL shaders compiled to SPIR-V.
- Key dirs:
  - `src/`: App skeleton, VTK reader, future Vulkan setup.
  - `include/`: Public headers (`app.h`, `vertex.h`, `vtk_reader.h`).
  - `data/`: User-provided VTK datasets. Agent must not commit large binaries.
  - `shaders/`: `.glsl` sources compiled via CMake to `.spv`.
  - `build/`: Out-of-source build folder.
  - `docs/`: Reports/videos (do not modify unless asked).

## Build & Run
- Primary workflow (from repo root):
  ```zsh
  mkdir -p build && pushd build
  cmake .. -DCMAKE_BUILD_TYPE=Debug
  cmake --build .
  ./VulkanCCOEngine
  popd
  ```
- Arch Linux packages (examples): `vulkan-headers`, `vulkan-icd-loader`, `vulkan-tools`, `shaderc`, `glfw`, `glm`.
- CMake finds Vulkan via CMake, pkg-config, or `$VULKAN_SDK`. If missing, it errors with guidance.
- Shaders: CMake compiles `shaders/*.glsl` using `glslc` and infers stage from filename (`vert`/`frag` or `.vert`/`.frag`).

## Current Application Flow (2D focus)
- Entry: `src/main.cpp` creates `App` and calls `run()`.
- `App::run()`:
  - Initializes window/Vulkan (placeholders).
  - Discovers `.vtk` files recursively under `DATA_DIR` (source `data/`; prefers `data/2D`).
  - Prints a numbered list and prompts for an index; loads the selected file via `VtkReader::load(..., false)`.
- `VtkReader`:
  - Reads ASCII VTK POLYDATA with `POINTS`, `LINES`, and optional `CELL_DATA` radii.
  - Produces `std::vector<ArterialSegment>` with `Vertex` endpoints and `radius`.

## Conventions & Patterns
- Headers in `include/` with simple, explicit data structures. Keep `Vertex` fields: `position[3]`, `normal[3]`, `color[4]`, `uv[2]`.
- Prefer explicit, safe parsing in `src/vtk_reader.cpp`; handle absent radii by defaulting.
- Avoid committing large `data/` files or generated `shaders/*.spv`.
- Use `DATA_DIR` macro to locate runtime data from the source tree; do not rely on build dir relative paths.
- Keep CMake minimal and explicit; add new sources under `set(SRC ...)`.

## Academic Requirements (TP1/TP2)
Phase I (TP1 – 2D):
- Projection: Use Orthographic Projection for all views (glOrtho conceptual equivalent). Implement via an orthographic `proj` matrix in the vertex shader.
- Geometry: Support fixed- and variable-radius cylindrical segments for arteries (radius sourced from VTK `CELL_DATA`).
- Transformations: Interactive 2D Translation/Rotation/Scale using homogeneous coordinates (4x4 matrices) applied in vertex processing (MVP).
- Interactivity: Incremental visualization using partial VTK files to reflect arterial growth.
- Bonus: Optionally implement line clipping (Cohen–Sutherland, Liang–Barsky, or Cyrus–Beck) to clip outside viewport.

Phase II (TP2 – 3D):
- Modeling: Branches modeled as 3D cylinders/tubes with variable thickness using efficient primitives (conceptual GL_TRIANGLE_STRIP/GL_TRIANGLE_FAN equivalents in Vulkan pipeline).
- Projection/Camera: Perspective Projection and observer control (orbit and free-look camera).
- Illumination: Implement at least two local models: Flat, Gouraud, or Phong.
- Normals: Compute and store per-vertex or per-face normals. If non-uniform scaling is used, transform normals by the inverse transpose of the Model-View matrix in the vertex shader.
- Realism: Transparency (blending) and hidden surface removal via Z-buffer.
- Bonus: Optional Ray Tracing/Ray Casting.

## Vulkan Architecture & Performance
- Vulkan Version: Target Vulkan 1.3.
- Pipeline: Prefer Dynamic Rendering (`VK_KHR_dynamic_rendering`) over legacy `VkRenderPass`/`VkFramebuffer` for simplicity and performance.
- Memory: Integrate Vulkan Memory Allocator (VMA) for all buffers/images. Initialize VMA after `VkDevice` creation; include `#define VMA_IMPLEMENTATION` once in a single `.cpp`.
- Debugging: Enable validation layers (`VK_LAYER_KHRONOS_validation`) and `VK_EXT_debug_utils` during development.
- Clearing: Use `VK_ATTACHMENT_LOAD_OP_CLEAR` at render start (or `vkCmdClearColorImage` when applicable).
- Swapchain: Handle `VK_ERROR_OUT_OF_DATE_KHR` / `VK_SUBOPTIMAL_KHR` by recreating the swapchain on resize/stale.

## Vertex Input & Shader Attributes
- Vertex mapping: `position` → location 0, `normal` → location 1, `color` → location 2. Use `VK_FORMAT_R32G32B32_SFLOAT` for vec3 attributes.
- MVP upload: Provide Model–View–Projection to the vertex shader via push constants (for small frequent updates) or a UBO.

## Adding Features (Examples)
- CLI selection:
  - Add parsing in `main.cpp` or `App` to accept `--data <path>`; override discovery.
  - Add `--list` to print candidates and exit.
- GLFW + Vulkan init:
  - Implement window creation in `App::initWindow()` via GLFW.
  - Create Vulkan `VkInstance`, select physical device, logical device, and surface/swapchain.
  - Place handles as private members in `App` (no one-letter names).
- 2D render path:
  - Build a simple pipeline to draw line segments from `segments2D` with orthographic projection.
  - Add input callbacks for translation/rotation/scale.

## Testing & Debugging
- Build-only tests: run the executable and verify discovery logs.
- Shader compile issues: ensure filenames include `vert`/`frag` or `.vert`/`.frag` to infer stage.
- VTK parser: log counts of points/lines/radii; handle malformed lines defensively.

## Do/Don’t for Agents
- Do: Keep changes scoped; maintain existing style; update CMake and headers consistently.
- Do: Use `std::filesystem` for paths; prefer absolute paths via `DATA_DIR`.
- Don’t: Commit user data in `data/`; don’t add heavy dependencies without approval.
- Don’t: Reformat large files unnecessarily; avoid aspirational docs—focus on code-backed patterns.

## Key Files
- `src/app.cpp`, `include/app.h`: App lifecycle and data discovery.
- `src/vtk_reader.cpp`, `include/vtk_reader.h`: VTK parsing logic.
- `include/vertex.h`: Vertex attributes used across pipeline.
- `CMakeLists.txt`: Build config, dependency discovery, shader rules.
- `README.md`: High-level project phases and usage.

# Vulkan CCO Arterial Visualization Engine (BCC327)

This repository hosts the BCC327 TP1–TP2 project: an interactive visualization engine for CCO (Constrained Constructive Optimization) arterial tree models, built on the Vulkan API with C++. The project evolves from 2D visualization (TP1) to advanced 3D rendering (TP2), emphasizing low-level graphics programming and modern shader pipelines.

## Overview
- Implements a robust Vulkan graphics pipeline tailored to physiological arterial tree data.
- Reads and processes VTK-formatted inputs to render arterial structures interactively.
- Demonstrates a clear progression: 2D orthographic visualization → 3D perspective rendering.
- Designed as a portfolio-quality, low-level graphics engineering project.

## Technologies
- Vulkan (C++)
- GLSL shaders (compiled to SPIR-V)
- VTK data format
- GLFW (windowing) and GLM (math) as vendor dependencies

## Phase I — TP1: 2D Visualization
- VTK Data Reading: custom parser for arterial tree datasets (2D subset).
- Orthographic Projection: stable, pixel-consistent rendering.
- Interactive 2D Transformations: translation, rotation, and scaling via mouse/keyboard callbacks.
- Incremental Visualization: ability to load partial datasets for progressive inspection.

## Phase II — TP2: 3D Rendering
- Perspective Projection: depth-aware camera and projection matrices.
- Observer/Camera Control: first-person or orbit camera with input callbacks.
- 3D Modeling: cylinder-based representation for arterial segments and bifurcations.
- Illumination Models: Flat, Gouraud, or Phong shading via GLSL.
- Z-Buffer and Transparency: correct depth ordering and optional alpha blending.

## Repository Structure
- `src/`: Core C/C++ source (Vulkan setup, pipelines, transforms, input handling).
- `include/`: Headers for vertices, VTK parsing, and engine subsystems.
- `data/`: VTK input files for 2D and 3D arterial models (include partials).
- `shaders/`: GLSL sources and compiled SPIR-V binaries.
- `build/`: Out-of-source CMake build directory and final executables.
- `docs/`: Technical reports (PDFs for TP1/TP2) and demonstration videos.
- `vendor/`: Third-party libraries (GLFW, GLM, and others as needed).

## Getting Started
1. Install Vulkan SDK and `glslc`.
2. Install dependencies (recommended: GLFW, GLM).
3. Configure CMake out-of-source build.

```zsh
# From project root
mkdir -p build && pushd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
./VulkanCCOEngine  # executable name placeholder
popd
```

## Shaders
- Place GLSL sources in `shaders/` (e.g., `vert.glsl`, `frag.glsl`).
- Compile to SPIR-V with `glslc` or `shaderc`.

```zsh
glslc -o shaders/vert.spv shaders/vert.glsl
glslc -o shaders/frag.spv shaders/frag.glsl
```

## Data
- Put VTK files in `data/`. Include 2D and 3D variants and partial datasets for incremental visualization.

## Deliverables
- `docs/TP1.pdf`: Technical report for Phase I.
- `docs/TP2.pdf`: Technical report for Phase II.
- `docs/videos/`: Demonstrations for both phases.

## Portfolio Value
By opting for Vulkan (a low-level, explicit graphics API) instead of a high-level fixed-function pipeline, this project showcases deep understanding of GPU pipelines, synchronization, memory management, and shader-based rendering—ideal for a systems-level graphics engineering portfolio.

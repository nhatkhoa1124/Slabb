# Slabb

A modular, real-time **graphics engine / renderer** written in modern C++20 on top of **DirectX 12**.

Slabb is structured as a small reusable engine library (`Slabb` shared library) plus a thin **sandbox** executable that demonstrates how to bootstrap a window, configure the renderer, and present a frame. It is currently in an early stage — focused on building the core rendering abstractions (render graph, resource lifetimes, pipelines, command recording) before piling on features.

---

## Highlights

- **C++20** engine, no STL/standard-library bloat in hot paths beyond what the language gives you.
- **DirectX 12** backend (Windows-only) with a hand-rolled wrapper layer around DXGI, the device, swapchain, command queues/lists/allocators, descriptor heaps, root signatures, fences, and pipeline state objects.
- **Render Graph** abstraction with explicit `RenderResource` / `RenderPass` declarations, automatic resource state tracking, and producer/consumer analysis for barrier insertion.
- **D3D12 Memory Allocator (D3D12MA)** for GPU buffer and texture allocations.
- **GLFW** for window/input — the platform/HWND bridge is encapsulated in a `Window` wrapper that hands a native `HWND` to the renderer backend.
- **HLSL shaders** compiled at build / load time via `d3dcompiler`.
- **tomlplusplus** for application configuration (`assets/config/application_cfg.toml`).
- **spdlog** for logging.
- **GoogleTest** wired up and ready (the `tests/` directory is reserved for it).

---

## Project Layout

```
Slabb/
├── CMakeLists.txt              # Root build script (CMake ≥ 3.20)
├── CMakeSettings.json          # Visual Studio / Ninja preset (x64-Debug)
├── cmake/
│   └── dependencies.cmake      # FetchContent for all third-party deps
├── include/                    # Public engine headers (mirrors src/ layout)
│   ├── common/                 # Cross-cutting macros (SLABB_EXPORT, etc.)
│   ├── core/
│   │   ├── application.hpp     # Top-level engine Application class
│   │   ├── core_interface/     # Vertex / vertex-attribute interface
│   │   ├── model/              # Mesh + Model data model
│   │   └── systems/            # ConfigSystem, RenderSystem
│   └── graphics/
│       ├── renderer.hpp        # High-level Renderer facade
│       ├── render_graph.hpp    # RenderGraph / RenderPass / RenderResource
│       ├── graphics_interface/ # GPU-side vertex layout
│       ├── tools/              # Validation + string utilities
│       └── wrapper/            # DX12 wrapper layer
│           ├── command/        # Allocator, list, queue
│           ├── pipeline/       # GraphicsPipeline (PSO)
│           ├── resource/       # DescriptorHeap, ResourceHeap (D3D12MA)
│           ├── synchronization/# Fence
│           └── window/         # GLFW ↔ HWND bridge
├── src/                        # Engine implementation (compiled into Slabb.dll/lib)
│   ├── core/
│   └── graphics/
├── sandbox/                    # Demo executable consuming the engine
│   ├── CMakeLists.txt
│   └── main.cpp
├── shaders/
│   ├── main.vertex.hlsl
│   └── main.pixel.hlsl
├── assets/
│   └── config/
│       └── application_cfg.toml
├── tests/                      # Reserved for GoogleTest cases
├── out/                        # Default build output (Visual Studio preset)
├── cmake-build-debug/          # Default build output (standalone CMake preset)
├── LICENSE.txt
└── .gitignore / .gitattributes
```

---

## Architecture

The engine is split into two cooperating layers plus a sandbox app:

### Core (`slabb::core`)
Engine-level state and orchestration. Independent of any graphics API:
- `Application` — owns the window, configuration system, and render system; drives the main loop.
- `ConfigSystem` — loads `assets/config/application_cfg.toml` via tomlplusplus and exposes typed `AppConfig` / `RendererConfig` structs (window title/size/mode, shader file lists).
- `RenderSystem` — adapts the engine-side `core::model::Model` into `GraphicsModel` data and dispatches draw calls to the renderer.
- `core::model` — pure data: `Model` is a collection of `Mesh`es; a `Mesh` is a list of `Vertex` + index buffer.

### Graphics (`slabb::graphics`)
The DirectX 12 backend, fronted by an API-neutral facade:
- `Renderer` — the only object the `RenderSystem` needs to talk to. Owns the DX12 instance/device/swapchain, per-frame command allocators and fences, descriptor heaps, root signature, and graphics pipeline.
- `RenderGraph` — declarative frame graph. You `add_pass(...)`, declare which `RenderResource`s each pass reads/writes, record command-list callbacks, and call `compile()` once at startup. At draw time `render(cmd_list)` walks the topologically sorted passes, inserts the right resource barriers, and executes them.
- `RenderResource` / `BufferResource` / `TextureResource` — GPU resource handles with tracked `D3D12_RESOURCE_STATES`; lifetimes are owned by the graph.
- `RenderPass` — a single node in the graph. Binds a PSO, root signature, viewport/scissor, and a command-list callback (`std::function`).
- `wrapper/` — thin RAII-ish wrappers around raw DX12 / DXGI / D3D12MA handles (`Instance`, `Device`, `Swapchain`, `CommandQueue`, `CommandList`, `CommandAllocator`, `DescriptorHeap`, `RootSignature`, `GraphicsPipeline`, `Fence`, `Window`).

### Sandbox (`sandbox/`)
Minimal end-to-end demo wired in via the `SLABB_BUILD_APP` CMake option. It instantiates `slabb::core::Application`, pushes a hard-coded colored quad mesh, and spins the main loop.

---

## Prerequisites

- **Windows 10/11** with the **DirectX 12** runtime (any reasonably modern GPU/driver).
- **Visual Studio 2022** (MSVC, x64) with the *Desktop development with C++* workload — or any toolchain with a working **CMake ≥ 3.20** + Ninja.
- **Windows SDK** (10.0 or newer) — provides DXGI, `d3dcompiler`, and the DirectX headers.
- **Git** — required by `FetchContent` to pull dependencies on first configure.

All third-party libraries (D3D12MA, GLFW, spdlog, tomlplusplus, GoogleTest, DirectX-Headers) are fetched automatically at configure time — you do **not** need to install them yourself.

---

## Building

### Quick start (Visual Studio)

`CMakeSettings.json` ships with a ready-to-use `x64-Debug` preset that enables the sandbox app. Just open the folder in Visual Studio 2022 as a CMake project and build.

### Command line (Ninja)

```bash
# From the repo root
cmake -S . -B cmake-build-debug -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DSLABB_BUILD_APP=ON
cmake --build cmake-build-debug
```

Build outputs go to `cmake-build-debug/bin/`. If `SLABB_BUILD_APP=ON`, the build additionally copies `assets/` and `shaders/` next to `slabb-app.exe` via a post-build step (see `sandbox/CMakeLists.txt`).

### CMake options

| Option              | Default | Description                                                  |
|---------------------|---------|--------------------------------------------------------------|
| `SLABB_BUILD_APP`   | `OFF`   | Build the `sandbox/` demo executable that links the engine. |

Toggle with `-DSLABB_BUILD_APP=ON` at configure time.

---

## Running the Sandbox

After a successful build with `SLABB_BUILD_APP=ON`:

```bash
./cmake-build-debug/bin/slabb-app.exe
```

The app reads `assets/config/application_cfg.toml` from the working directory (or from the path next to the executable, thanks to the post-build copy step) and opens a 1280×900 window titled **"Slabb"**. A single colored quad is uploaded as a placeholder mesh — the demo is intended as a smoke test for the renderer pipeline, not a scene.

Edit `assets/config/application_cfg.toml` to tweak window size, mode (windowed/borderless), resizability, visibility, or shader paths:

```toml
[application.window]
title = "Slabb"
width = 1280
height = 900
mode = "windowed"
resizable = false
visible = true

[renderer.shader]
vertex_files = [ "shaders/main.vertex.hlsl" ]
pixel_files = [ "shaders/main.pixel.hlsl" ]
```

---

## Coding Conventions (quick map)

- Namespace layout: `slabb::core::*` for engine state, `slabb::graphics::*` for the rendering facade, `slabb::graphics::wrapper::*` for DX12 wrappers.
- Public symbols intended for cross-DLL use are tagged with `SLABB_EXPORT` (defined in `include/common/common.hpp`).
- All engine translation units target **C++20** (`CXX_STANDARD 20`, `CXX_EXTENSIONS OFF`).
- Headers live under `include/`, implementations under `src/`, mirrored 1:1 where it makes sense.
- Logging goes through **spdlog**; don't sprinkle `printf`/`std::cout` in engine code.
- Config goes through **tomlplusplus**, not magic constants in source.

---

## Dependencies

Pulled in automatically via `FetchContent` in [`cmake/dependencies.cmake`](cmake/dependencies.cmake):

| Library          | Version  | Purpose                                     |
|------------------|----------|---------------------------------------------|
| D3D12MA          | v3.1.0   | Sub-allocator for D3D12 buffers/textures    |
| GLFW             | 3.4      | Window creation + input                     |
| spdlog           | v1.17.0  | Logging                                     |
| tomlplusplus     | v3.4.0   | Application configuration parsing           |
| GoogleTest       | v1.17.0  | Test framework                              |
| DirectX-Headers  | v1.619.1 | Microsoft DirectX C++ headers               |

DXGI, `d3d12`, and `d3dcompiler` are linked from the Windows SDK.

---

## Roadmap (informal, not a commitment)

- Proper scene format / asset loader (the current mesh is hard-coded in `application.cpp`).
- Depth buffer + depth-tested geometry.
- Texture / SRV pipeline (descriptor heap infrastructure is in place).
- Synchronous render-graph caching and persistent descriptors.
- Hot-reload for shaders and config.
- Actual unit tests in `tests/`.

---

## License

Released under the **MIT License** — see [`LICENSE.txt`](LICENSE.txt).
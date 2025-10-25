# Synty‑Lite

Minimal, hot‑reloadable 3D “drop‑folder” engine that demonstrates a GPU driven render loop and a tiny asset pipeline.  The project is intentionally small – no editor, no game objects, just enough code to prove the core open‑world loop:

```
Drop raw assets → importer auto‑atlases & merges → renderer hot‑loads →
GPU frustum culling → single indirect draw
```

The sample shows a grid of instances (unit cubes by default).  
Dropping an FBX/GLTF/OBJ + textures into `Content/Drop/` will trigger the importer on the next poll, bake a prefab inside `Content/Packs/`, hot‑load it into the running app and append an instance to the world.

---

## Build instructions

All code is C++17 and builds on **Linux** and **Windows** using **CMake**.

### 1. Fetch third‑party libraries

Required packages:

* GLFW – window/input  
* Volk – Vulkan function loader  
* GLM – math library  
* VMA – GPU memory allocator  
* Assimp – asset importing  
* meshoptimizer – mesh optimisations  
* stb – image loading / atlas / image write

From repo root run:

```bash
./scripts/setup_deps.sh        # Linux
# or
scripts\setup_deps.bat         # Windows (run in VS dev cmd)
```

This will clone all dependencies into `external/`.

### 2. Configure & build

```bash
mkdir build
cd build
cmake ..                       # -DSYNTY_LITE_BUILD_TESTS=ON to build tests
cmake --build .
```

If `glslc` is present it will be used at build time to compile shaders; otherwise the GLSL files are copied and compiled at runtime.

#### Windows quick start

1. Install the **Vulkan SDK** (adds the `VULKAN_SDK` environment variable that CMake and Volk use).
2. Install **Visual Studio 2022** (or the Build Tools) with the *Desktop development with C++* workload.
3. Open a **x64 Native Tools Command Prompt for VS 2022** so the toolchain is on `PATH`.
4. From the repo root run the dependency script:

   ```bat
   scripts\setup_deps.bat
   ```

5. Configure and build with CMake:

   ```bat
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64
   cmake --build build --config Release
   ```

   (Use `--config Debug` when you need debug symbols.)

The generated solution lives in `build/synty-lite.sln` if you prefer to build from within Visual Studio.

### 3. Run

```
./synty-lite
```

*With no assets dropped* the application generates a grid of unit cubes – the binary is loaded from `assets/unit_cube.bin` or procedurally generated if the file is missing.

### 4. Importing assets

1. Copy your model (FBX/GLB/GLTF/OBJ) and textures into `Content/Drop/`.
2. The watcher (checks every ~2 s) detects the change and calls the importer.
3. Imported data is written to `Content/Packs/<PrefabName>/`:
   * `mesh_opaque.bin`
   * `mesh_transparent.bin` *(optional)*
   * `atlas_albedo.png`
   * `materials.json`
   * `bounds.json`
4. When the importer finishes the prefab is hot‑loaded into the renderer and an instance is added to the world.  `Content/Worlds/autogrid.world.json` keeps persistent world layout.

---

## Project layout
```
synty-lite/
  CMakeLists.txt
  README.md
  LICENSE
  external/              # third‑party libraries after setup_deps
  scripts/
    setup_deps.sh
    setup_deps.bat
  shaders/               # GLSL files (compiled at build/run)
  assets/
    unit_cube.bin        # optional; generated if missing
  Content/
    Drop/                # put your source assets here
    Packs/               # importer writes baked assets here
    Worlds/autogrid.world.json
  src/                   # all engine code
```

---

## License

MIT – see [LICENSE](LICENSE).

---

### Optional flags

* `--validation` – enable Vulkan validation layers (if installed).
* `--fps` – print FPS counter in console.

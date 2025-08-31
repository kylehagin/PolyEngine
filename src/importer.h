#pragma once
#include <filesystem>

// Import a source asset (fbx/obj/gltf/glb) into the engine's pack format.
// Returns true on success.
bool ImportAssetFile(const std::filesystem::path& inputFile,
                     const std::filesystem::path& outputDir,
                     int atlasSize = 4096, int padding = 4);

#include "importer.h"
#include "log.h"
#include "fs_util.h"
#include "unit_cube.h"

// The real implementation would use Assimp + stb + meshoptimizer.
// For brevity this stub simply copies a unit cube as the baked asset.
bool ImportAssetFile(const std::filesystem::path& inputFile,
                     const std::filesystem::path& outputDir,
                     int, int)
{
    Log("Import stub: %s", inputFile.string().c_str());
    EnsureDir(outputDir);
    WriteUnitCubeBin(outputDir / "mesh_opaque.bin");
    return true;
}

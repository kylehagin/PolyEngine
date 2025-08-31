#include "unit_cube.h"
#include "pack_formats.h"
#include "fs_util.h"
#include <vector>
#include <cstring>

struct Vertex { float px,py,pz; float nx,ny,nz; float u,v; };

bool WriteUnitCubeBin(const std::filesystem::path& path)
{
    Vertex verts[24] = {
        // +X
        {1,-1,-1, 1,0,0, 1,0}, {1,1,-1, 1,0,0, 1,1}, {1,1,1, 1,0,0, 0,1}, {1,-1,1, 1,0,0, 0,0},
        // -X
        {-1,-1,1,-1,0,0,1,0}, {-1,1,1,-1,0,0,1,1}, {-1,1,-1,-1,0,0,0,1}, {-1,-1,-1,-1,0,0,0,0},
        // +Y
        {-1,1,-1,0,1,0,0,0}, {-1,1,1,0,1,0,0,1}, {1,1,1,0,1,0,1,1}, {1,1,-1,0,1,0,1,0},
        // -Y
        {-1,-1,1,0,-1,0,0,0}, {-1,-1,-1,0,-1,0,0,1}, {1,-1,-1,0,-1,0,1,1}, {1,-1,1,0,-1,0,1,0},
        // +Z
        {-1,-1,1,0,0,1,0,0}, {1,-1,1,0,0,1,1,0}, {1,1,1,0,0,1,1,1}, {-1,1,1,0,0,1,0,1},
        // -Z
        {1,-1,-1,0,0,-1,0,0}, {-1,-1,-1,0,0,-1,1,0}, {-1,1,-1,0,0,-1,1,1}, {1,1,-1,0,0,-1,0,1}
    };
    uint32_t idx[36] = {
        0,1,2,0,2,3, 4,5,6,4,6,7,
        8,9,10,8,10,11, 12,13,14,12,14,15,
        16,17,18,16,18,19, 20,21,22,20,22,23
    };
    MeshHeader hdr{0x3048534D, 24, 36, sizeof(Vertex)};
    std::vector<char> data(sizeof(MeshHeader)+sizeof(verts)+sizeof(idx));
    memcpy(data.data(), &hdr, sizeof(hdr));
    memcpy(data.data()+sizeof(hdr), verts, sizeof(verts));
    memcpy(data.data()+sizeof(hdr)+sizeof(verts), idx, sizeof(idx));
    return WriteFileBinary(path, data.data(), data.size());
}


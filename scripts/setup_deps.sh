#!/usr/bin/env bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EXT="$ROOT/external"
mkdir -p "$EXT"

clone () {
    if [ ! -d "$EXT/$2" ]; then
        git clone --depth 1 "$1" "$EXT/$2"
    fi
}

clone https://github.com/glfw/glfw.git glfw
clone https://github.com/zeux/volk.git volk
clone https://github.com/g-truc/glm.git glm
clone https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git vma
clone https://github.com/assimp/assimp.git assimp
clone https://github.com/zeux/meshoptimizer.git meshoptimizer
clone https://github.com/nothings/stb.git stb

echo "Dependencies checked out in $EXT"

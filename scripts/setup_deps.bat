@echo off
set ROOT=%~dp0..
set EXT=%ROOT%\external
if not exist %EXT% mkdir %EXT%

call :clone https://github.com/glfw/glfw.git glfw
call :clone https://github.com/zeux/volk.git volk
call :clone https://github.com/g-truc/glm.git glm
call :clone https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git vma
call :clone https://github.com/assimp/assimp.git assimp
call :clone https://github.com/zeux/meshoptimizer.git meshoptimizer
call :clone https://github.com/nothings/stb.git stb
echo Dependencies checked out in %EXT%
goto :EOF

:clone
if not exist %EXT%\%2 (
    git clone --depth 1 %1 %EXT%\%2
)
goto :EOF

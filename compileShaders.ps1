md -Force Binary\shaders > $null
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=vert -o Binary\shaders\terrainvert.spv src\graphics\vulkan\shaders\terrain.vert
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=frag -o Binary\shaders\terrainfrag.spv src\graphics\vulkan\shaders\terrain.frag
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=vert -o Binary\shaders\texturevert.spv src\graphics\vulkan\shaders\texture.vert
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=frag -o Binary\shaders\texturefrag.spv src\graphics\vulkan\shaders\texture.frag
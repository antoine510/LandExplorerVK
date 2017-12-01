md -Force Binary\shaders > $null
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=vert -o Binary\shaders\terrainvert.spv src\graphics\vulkan\shaders\terrain.vert
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=frag -o Binary\shaders\terrainfrag.spv src\graphics\vulkan\shaders\terrain.frag
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=vert -o Binary\shaders\spritevert.spv src\graphics\vulkan\shaders\sprite.vert
& $Env:VULKAN_SDK\Bin\glslc.exe -fshader-stage=frag -o Binary\shaders\spritefrag.spv src\graphics\vulkan\shaders\sprite.frag
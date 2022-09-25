cmake_minimum_required(VERSION 3.5)
project(LandExplorerVK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("src" "Vulkan/Include" "SDL2-2.0.8/include" "Lua-5.3.5/include" "FMOD/include")

if(WIN32)
	link_directories("SDL2-2.0.8/lib/Win64" "Lua-5.3.5/lib/Win64" "Vulkan/Lib/Win64" "FMOD/lib/Win64")
	add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS)
else()
endif ()


add_subdirectory(src)


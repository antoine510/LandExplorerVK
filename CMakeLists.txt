cmake_minimum_required(VERSION 3.5)
project(LandExplorerVK)

include_directories("src" "Vulkan/Include" "SDL2-2.0.7/include" "libxml2/include" "FMOD/include")

if(WIN32)
	if(CMAKE_GENERATOR MATCHES "Win64")
		link_directories("SDL2-2.0.7/lib/Win64" "libxml2/lib/Win64" "Vulkan/Lib/Win64" "FMOD/lib/Win64")
	else()
		message( FATAL_ERROR "Win64 is the only supported configuration")
	endif()
	add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS)
elseif (APPLE)
else()
	add_definitions()
	link_directories()
endif ()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CPP11_FLAG "/EHsc")
else()
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
	check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
	check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX14)
		set(CPP11_FLAG "-std=c++14")
	elseif(COMPILER_SUPPORTS_CXX11)
		set(CPP11_FLAG "-std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set(CPP11_FLAG "-std=c++0x")
	else()
		MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. This can result in build failure.")
	endif()
endif()

add_definitions("/MP" "/std:c++17")

add_subdirectory(src)


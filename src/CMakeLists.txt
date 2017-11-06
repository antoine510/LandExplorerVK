file(GLOB SRC_FILES
	"*.cpp"
	"*.h"
)
	
file(GLOB SRC_AI
	"ai/*.cpp"
	"ai/*.h"
)

file(GLOB SRC_GRAPHICS
	"graphics/*.cpp"
	"graphics/*.h"
)

file(GLOB SRC_GRAPHICS
	"graphics/*.cpp"
	"graphics/*.h"
)

file(GLOB SRC_GRAPHICS_OPENGL
	"graphics/opengl/*.cpp"
	"graphics/opengl/*.h"
)

file(GLOB SRC_GRAPHICS_VULKAN
	"graphics/vulkan/*.cpp"
	"graphics/vulkan/*.h"
)

file(GLOB SRC_GRAPHICS_RENDERER
	"graphics/renderers/*.cpp"
	"graphics/renderers/*.h"
)

file(GLOB SRC_PHYSICS
	"physics/*.cpp"
	"physics/*.h"
)

file(GLOB SRC_PLAYER
	"player/*.cpp"
	"player/*.h"
)

file(GLOB SRC_TERRAIN
	"terrain/*.cpp"
	"terrain/*.h"
)

file(GLOB SRC_UI
	"ui/*.cpp"
	"ui/*.h"
)

file(GLOB SRC_UTILITY
	"utility/*.cpp"
	"utility/*.h"
)

add_executable(LandExplorerVK ${SRC_FILES} ${SRC_AI} ${SRC_GRAPHICS}
	${SRC_GRAPHICS_VULKAN} ${SRC_GRAPHICS_RENDERER} ${SRC_PHYSICS} ${SRC_PLAYER}
	${SRC_TERRAIN} ${SRC_UI} ${SRC_UTILITY})

source_group(AI						FILES ${SRC_AI})
source_group(Graphics				FILES ${SRC_GRAPHICS})
source_group(Graphics\\Vulkan		FILES ${SRC_GRAPHICS_VULKAN})
source_group(Graphics\\Renderers	FILES ${SRC_GRAPHICS_RENDERER})
source_group(Physics				FILES ${SRC_PHYSICS})
source_group(Player					FILES ${SRC_PLAYER})
source_group(Terrain				FILES ${SRC_TERRAIN})
source_group(UI						FILES ${SRC_UI})
source_group(Utility				FILES ${SRC_UTILITY})

target_link_libraries(LandExplorerVK "SDL2" "SDL2main" "SDL2_ttf" "SDL2_image" "libxml2" "fmodL")

if(WIN32)
	target_link_libraries(LandExplorerVK "vulkan-1")
elseif (APPLE)
else()
	target_link_libraries(LandExplorerVK "vulkan")
endif ()

set_target_properties(LandExplorerVK
    PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_RELEASE 			"${CMAKE_SOURCE_DIR}/Binary"
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO 	"${CMAKE_SOURCE_DIR}/Binary"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG 				"${CMAKE_SOURCE_DIR}/Binary"
	VS_DEBUGGER_WORKING_DIRECTORY				"${CMAKE_SOURCE_DIR}/Binary"
)


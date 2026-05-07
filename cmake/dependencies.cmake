include(FetchContent)

FetchContent_Declare(d3d12ma SYSTEM
	GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
	GIT_TAG v3.1.0
	GIT_SHALLOW ON
	GIT_PROGRESS ON
	FIND_PACKAGE_ARGS 3.1.0
)

FetchContent_Declare(glfw SYSTEM
	GIT_REPOSITORY https://github.com/glfw/glfw
	GIT_TAG 3.4
	GIT_SHALLOW ON
	GIT_PROGRESS ON 
	FIND_PACKAGE_ARGS 3.4
)

FetchContent_Declare(spdlog SYSTEM
	GIT_REPOSITORY https://github.com/gabime/spdlog
	GIT_TAG v1.17.0 
	GIT_SHALLOW ON 
	GIT_PROGRESS ON
	FIND_PACKAGE_ARGS 1.17.0
)
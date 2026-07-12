include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(d3d12ma SYSTEM
	GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
	GIT_TAG v3.1.0
	GIT_SHALLOW ON
	GIT_PROGRESS ON
)

FetchContent_Declare(glfw SYSTEM
	GIT_REPOSITORY https://github.com/glfw/glfw
	GIT_TAG 3.4
	GIT_SHALLOW ON
	GIT_PROGRESS ON 
)

FetchContent_Declare(spdlog SYSTEM
	GIT_REPOSITORY https://github.com/gabime/spdlog
	GIT_TAG v1.17.0 
	GIT_SHALLOW ON 
	GIT_PROGRESS ON
)

FetchContent_Declare(tomlplusplus
	GIT_REPOSITORY https://github.com/marzer/tomlplusplus
	GIT_TAG v3.4.0
	GIT_SHALLOW ON 
	GIT_PROGRESS ON
)

FetchContent_Declare(gtest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG v1.17.0
	GIT_SHALLOW ON
	GIT_PROGRESS ON 
)

FetchContent_Declare(directx_headers
	GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers 
	GIT_TAG v1.619.1
	GIT_SHALLOW ON 
	GIT_PROGRESS ON 
)

FetchContent_Declare(tinygltf
	GIT_REPOSITORY https://github.com/syoyo/tinygltf
	GIT_TAG v3.0.0 
	GIT_SHALLOW ON
	GIT_PROGRESS ON
)

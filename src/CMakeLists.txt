set(SLABB_SOURCE_FILES
	graphics/render_graph.cpp
	graphics/renderer.cpp
	graphics/wrapper/device.cpp
	graphics/wrapper/instance.cpp
	core/application.cpp 
	graphics/wrapper/window/window.cpp
	graphics/tools/validation.cpp 
)

add_library(${PROJECT_NAME} SHARED ${SLABB_SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_EXTENSIONS OFF
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	d3d12
	dxgi
	d3dcompiler
	D3D12MemoryAllocator
	glfw
	spdlog::spdlog
	tomlplusplus::tomlplusplus
)
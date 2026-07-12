set(SLABB_SOURCE_FILES
	graphics/render_graph.cpp
	graphics/renderer.cpp
	graphics/wrapper/device.cpp
	graphics/wrapper/instance.cpp
	core/application.cpp 
	graphics/wrapper/window/window.cpp
	graphics/tools/validation.cpp 
	graphics/wrapper/command/command_queue.cpp
	graphics/wrapper/swapchain.cpp
	graphics/wrapper/resource/descriptor_heap.cpp
	graphics/wrapper/command/command_allocator.cpp
	graphics/wrapper/root_signature.cpp
	graphics/wrapper/pipeline/graphics_pipeline.cpp
	graphics/wrapper/shader_manager.cpp
	core/core_interface/core_vertex.cpp 
	graphics/graphics_interface/graphics_vertex.cpp 
	graphics/wrapper/command/command_list.cpp
	core/systems/render_system.cpp
	core/systems/config_system.cpp
	core/model/model.cpp
	graphics/wrapper/resource/resource_heap.cpp 
	graphics/wrapper/synchronization/fence.cpp 
	graphics/graphics_interface/graphics_shader.cpp 
	graphics/scene.cpp 
	graphics/render_pipeline.cpp
	graphics/context.cpp 
	graphics/render_queue.cpp
	graphics/texture_manager.cpp)

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
	Microsoft::DirectX-Headers
	tinygltf
)
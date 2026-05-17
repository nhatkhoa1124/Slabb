cmake_minimum_required(VERSION 3.20)
project(Slabb CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)


# Build a minimal app for demonstration
option(SLABB_BUILD_APP "Demo App Build" OFF)

include(cmake/dependencies.cmake)

FetchContent_MakeAvailable(
	d3d12ma 
	glfw
	spdlog
	tomlplusplus
	gtest
	directx_headers
)

add_subdirectory(src)

if(SLABB_BUILD_APP)
	add_subdirectory(sandbox)
endif()




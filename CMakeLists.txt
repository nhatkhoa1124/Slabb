cmake_minimum_required(VERSION 3.20)
project(Slabb CXX)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Build a minimal app for demonstration
option(SLABB_BUILD_APP "Demo App Build" OFF)

include(cmake/dependencies.cmake)

add_subdirectory(src)

if(SLABB_BUILD_APP)
	add_subdirectory(sandbox)
endif()




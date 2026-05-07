
add_library(${PROJECT_NAME} "graphics/render_graph.cpp" "graphics/renderer.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES
CXX_EXTENSIONS OFF
CXX_STANDARD 20
CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME} PUBLIC
${PROJECT_SOURCE_DIR}/include
)

FetchContent_MakeAvailable(d3d12ma)
FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(spdlog)
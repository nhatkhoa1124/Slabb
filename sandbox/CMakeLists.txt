set(APP_SOURCE_FILES
	main.cpp
)

file(GLOB SHADER_FILES "${PROJECT_SOURCE_DIR}/shaders/*.hlsl")

add_executable(slabb-app ${APP_SOURCE_FILES})

add_custom_command(TARGET slabb-app POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${PROJECT_SOURCE_DIR}/assets
			$<TARGET_FILE_DIR:slabb-app>/assets
			DEPENDS ${PROJECT_SOURCE_DIR}/assets
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${PROJECT_SOURCE_DIR}/shaders
			$<TARGET_FILE_DIR:slabb-app>/shaders
			DEPENDS ${SHADER_FILES}
	COMMENT "Deploying engine assets and shaders to build output directory..."
)

target_compile_features(slabb-app PRIVATE cxx_std_20)
target_link_libraries(slabb-app PRIVATE ${PROJECT_NAME})
target_compile_options(slabb-app PRIVATE "-EHs")
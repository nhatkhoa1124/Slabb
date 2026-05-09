set(APP_SOURCE_FILES
	main.cpp
)

add_executable(slabb-app ${APP_SOURCE_FILES})

add_custom_command(TARGET slabb-app POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${PROJECT_SOURCE_DIR}/assets
			$<TARGET_FILE_DIR:slabb-app>/assets
			DEPENDS ${ASSET_FILES}
)

target_compile_features(slabb-app PRIVATE cxx_std_20)
target_link_libraries(slabb-app PRIVATE ${PROJECT_NAME})
target_compile_options(slabb-app PRIVATE "-EHs")
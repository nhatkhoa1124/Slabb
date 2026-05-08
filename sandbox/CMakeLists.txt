set(APP_SOURCE_FILES
	main.cpp
)

add_executable(slabb-app ${APP_SOURCE_FILES})

target_compile_features(slabb-app PRIVATE cxx_std_20)
target_link_libraries(slabb-app PRIVATE ${PROJECT_NAME})
target_compile_options(slabb-app PRIVATE "-EHs")
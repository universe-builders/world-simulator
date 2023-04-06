cmake_minimum_required(VERSION 3.18.4)
project(active-passive-elector C)

# Source Files.
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/source/*.c)

# Target.
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

# Compile flags.
add_definitions("-g")
add_definitions("-Werror")

# Includes.
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/universe-builders)
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/third-party)

# Universe Builders Libs.
set(C_LIBS_LIB_PATH /usr/local/lib/universe-builders)
target_link_libraries(${PROJECT_NAME} ${C_LIBS_LIB_PATH}/libc-core.so)
target_link_libraries(${PROJECT_NAME} ${C_LIBS_LIB_PATH}/libc-ds.so)
target_link_libraries(${PROJECT_NAME} ${C_LIBS_LIB_PATH}/libc-net.so)
target_link_libraries(${PROJECT_NAME} ${C_LIBS_LIB_PATH}/libc-k8s.so)

# Kubernetes Libs.
set(K8s_LIB_PATH /usr/local/lib/third-party)
target_link_libraries(${PROJECT_NAME} ${K8s_LIB_PATH}/libkubernetes.so)
target_link_libraries(${PROJECT_NAME} ${K8s_LIB_PATH}/libyaml.so)
target_link_libraries(${PROJECT_NAME} ${K8s_LIB_PATH}/libwebsockets.so)

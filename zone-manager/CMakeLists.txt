cmake_minimum_required(VERSION 3.18.4)
project(zone-manager C)

set(SOURCE_DIR /root/zone-manager/src/)

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
#include(CMakePrintHelpers)
#cmake_print_variables(SOURCE_FILES)

# Local.
#add_subdirectory(src)
#target_include_directories(${PROJECT_NAME} PRIVATE src/)

# Kubernetes.
set(K8s_INCLUDE_PATH third-party/kubernetes-client/c/kubernetes)
target_include_directories(${PROJECT_NAME} PUBLIC ${K8s_INCLUDE_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${K8s_INCLUDE_PATH}/model)
target_include_directories(${PROJECT_NAME} PUBLIC ${K8s_INCLUDE_PATH}/api)
target_include_directories(${PROJECT_NAME} PUBLIC ${K8s_INCLUDE_PATH}/config)
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libkubernetes.so)
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libyaml.so)
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libwebsockets.so)
cmake_minimum_required(VERSION 3.18.4)
project(client C)

set(SOURCE_DIR /root/zone-manager/client/source/)

file(GLOB_RECURSE SOURCE_FILES FOLLOW_SYMLINKS ${SOURCE_DIR}*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
cmake_minimum_required(VERSION 3.18.4)
project(active-passive-elector C)

set(SOURCE_DIR /root/zone-manager/active-passive-elector/source/)

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
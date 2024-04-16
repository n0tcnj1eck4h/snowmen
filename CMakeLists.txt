cmake_minimum_required(VERSION 3.10)
project(raymarcher_project)


add_executable(raymarcher src/main.c src/glad.c src/utils.c)

add_subdirectory(SDL2)
include_directories(${SDL2_SOURCE_DIR}/include)
target_link_libraries(raymarcher PRIVATE SDL2-static SDL2main)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(raymarcher PUBLIC ${MATH_LIBRARY})
endif()

target_include_directories(raymarcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(raymarcher PRIVATE cglm_headers)
add_subdirectory(cglm/ EXCLUDE_FROM_ALL)

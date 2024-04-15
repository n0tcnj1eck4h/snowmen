cmake_minimum_required(VERSION 3.10)
project(raymarcher_project)

# Find SDL2 package
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL2")
find_package(SDL2 REQUIRED)

add_executable(raymarcher src/main.c src/glad.c src/utils.c)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(raymarcher PUBLIC ${MATH_LIBRARY})
endif()

target_include_directories(raymarcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(raymarcher PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(raymarcher PRIVATE ${SDL2_LIBRARIES})

target_link_libraries(raymarcher PRIVATE cglm_headers)
add_subdirectory(cglm/ EXCLUDE_FROM_ALL)

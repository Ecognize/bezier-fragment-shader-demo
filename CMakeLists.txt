cmake_minimum_required(VERSION 2.8)
project(bezier-fragshader)
find_package(OpenGL REQUIRED)

set(SDL_SHARED FALSE)
set(SDL_STATIC TRUE)
# set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)  # SDL doesn't a fuck
add_subdirectory(SDL)

add_definitions(-DDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
add_executable(bezier-fragshader bezier-fragshader.c)
include_directories(${SDL2_BINARY_DIR}/include ${SDL2_SOURCE_DIR}/include)
link_directories(${SDL2_BINARY_DIR}) # Isn't working?
target_link_libraries(bezier-fragshader ${OPENGL_LIBRARIES} ${SDL2-static} m dl pthread SDL2-static)

cmake_minimum_required(VERSION 2.8)
project(bezier-fragshader)

set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)  # SDL don't gives a fuck
add_subdirectory(SDL)

add_definitions(-DDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
add_executable(bezier-fragshader bezier-fragshader.c)
target_link_libraries(bezier-fragshader ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

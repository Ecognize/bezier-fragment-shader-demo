cmake_minimum_required(VERSION 2.8)
project(bezier-fragshader)
find_package(OpenGL REQUIRED)
find_package(GLUT)

if (GLUT_FOUND)
    # TODO make an override
    message("GLUT found, building a non-SDL version")
    set(AUXLIBS ${GLUT_LIBRARIES})
    include_directories(${GLUT_INCLUDE_DIR})
else()
    message("No GLUT found, using SDL.")
    set(AUXLIBS ${SDL2-static} m dl pthread SDL2-static)
    include_directories(${SDL2_BINARY_DIR}/include ${SDL2_SOURCE_DIR}/include)
    link_directories(${SDL2_BINARY_DIR}) # Isn't working?
    set(SDL_SHARED FALSE)
    set(SDL_STATIC TRUE)
    # set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)  # SDL doesn't a fuck
    add_subdirectory(SDL)
    add_definitions(-DUSE_SDL)
endif()

add_definitions(-DDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
add_executable(bezier-fragshader bezier-fragshader.c matrix-math.c matrix-math.h shape.h)
target_link_libraries(bezier-fragshader ${OPENGL_LIBRARIES} ${AUXLIBS})

#ifndef __GRAPHLIBS_H__

    #ifdef USE_SDL
    #include <SDL_config.h>
    #include <SDL.h>
    #ifdef GLES
    #include <SDL_opengles2.h>
    #else
    #include <SDL_opengl.h>
    #endif
    #else
        #ifdef _WIN32
            #include <windows.h> 
        #endif
        #include <GL/glut.h>
    #endif

#endif
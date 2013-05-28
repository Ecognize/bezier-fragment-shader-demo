#ifndef LUAR_MATRIX_MATH_H_
#define LUAR_MATRIX_MATH_H_

#include <math.h>
#include <stdio.h>

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

// So that it can be used as a return type
struct GLMatrix
{
    GLfloat data[16];
};

struct GLMatrix getGLMatrixProduct(struct GLMatrix a,struct GLMatrix b);
struct GLMatrix getGLTranslateMatrix(GLfloat dx,GLfloat dy,GLfloat dz);
struct GLMatrix getGLRotateMatrix(GLfloat angle,GLfloat _x,GLfloat _y,GLfloat _z);
struct GLMatrix getGLPerspectiveMatrix(GLfloat fov,GLfloat aspect,GLfloat zNear,GLfloat zFar);
struct GLMatrix getGLLookAtMatrix(GLfloat eyeX,GLfloat eyeY,GLfloat eyeZ,
                                  GLfloat centerX,GLfloat centerY,GLfloat centerZ,
                                  GLfloat upX,GLfloat upY,GLfloat upZ);

// For the debug only
void printGLMatrix(struct GLMatrix a);
struct GLMatrix captureGLMatrix(GLenum type); // This probably requires having a GL context

#endif // LUAR_MATRIX_MATH_H_
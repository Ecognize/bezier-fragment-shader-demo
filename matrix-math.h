#ifndef LUAR_MATRIX_MATH_H_
#define LUAR_MATRIX_MATH_H_

#include <math.h>
#include <stdio.h>
#include "graphlibs.h"

// So that it can be used as a return type
struct GLMatrix
{
    GLfloat data[16];
};

struct GLMatrix getGLIdentityMatrix();
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
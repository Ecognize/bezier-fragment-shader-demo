#include "matrix-math.h"

// Probably there's code for that, but it's fast enough to type
struct GLMatrix getGLMatrixProduct(struct GLMatrix a,struct GLMatrix b)
{
    struct GLMatrix ret={
        .data={     a.data[0] * b.data[0] + a.data[1] * b.data[4] + a.data[2] * b.data[8] + a.data[3] * b.data[12],
                    a.data[0] * b.data[1] + a.data[1] * b.data[5] + a.data[2] * b.data[9] + a.data[3] * b.data[13],
                    a.data[0] * b.data[2] + a.data[1] * b.data[6] + a.data[2] * b.data[10] + a.data[3] * b.data[14],
                    a.data[0] * b.data[3] + a.data[1] * b.data[7] + a.data[2] * b.data[11] + a.data[3] * b.data[15],
                    a.data[4] * b.data[0] + a.data[5] * b.data[4] + a.data[6] * b.data[8] + a.data[7] * b.data[12],
                    a.data[4] * b.data[1] + a.data[5] * b.data[5] + a.data[6] * b.data[9] + a.data[7] * b.data[13],
                    a.data[4] * b.data[2] + a.data[5] * b.data[6] + a.data[6] * b.data[10] + a.data[7] * b.data[14],
                    a.data[4] * b.data[3] + a.data[5] * b.data[7] + a.data[6] * b.data[11] + a.data[7] * b.data[15],
                    a.data[8] * b.data[0] + a.data[9] * b.data[4] + a.data[10] * b.data[8] + a.data[11] * b.data[12],
                    a.data[8] * b.data[1] + a.data[9] * b.data[5] + a.data[10] * b.data[9] + a.data[11] * b.data[13],
                    a.data[8] * b.data[2] + a.data[9] * b.data[6] + a.data[10] * b.data[10] + a.data[11] * b.data[14],
                    a.data[8] * b.data[3] + a.data[9] * b.data[7] + a.data[10] * b.data[11] + a.data[11] * b.data[15],
                    a.data[12] * b.data[0] + a.data[13] * b.data[4] + a.data[14] * b.data[8] + a.data[15] * b.data[12],
                    a.data[12] * b.data[1] + a.data[13] * b.data[5] + a.data[14] * b.data[9] + a.data[15] * b.data[13],
                    a.data[12] * b.data[2] + a.data[13] * b.data[6] + a.data[14] * b.data[10] + a.data[15] * b.data[14],
                    a.data[12] * b.data[3] + a.data[13] * b.data[7] + a.data[14] * b.data[11] + a.data[15] * b.data[15],
        },
    };
    return ret;
}

struct GLMatrix getGLTranslateMatrix(GLfloat dx,GLfloat dy,GLfloat dz)
{
    struct GLMatrix ret={
        .data={ 1,  0,  0,  dx,
                0,  1,  0,  dy,
                0,  0,  1,  dz,
                0,  0,  0,  1,
        },
    };
    return ret;
}

struct GLMatrix getGLRotateMatrix(GLfloat angle,GLfloat _x,GLfloat _y,GLfloat _z)
{
    GLfloat len=sqrt(_x*_x+_y*_y+_z*_z);
    GLfloat x=_x/len, y=_y/len, z=_z/len, c=cos(angle), s=sin(angle);
    struct GLMatrix ret={
        .data={ x*x*(1-c)+c,    x*y*(1-c)-z*s,  x*z*(1-c)+y*s,  0,
                y*x*(1-c)+z*s,  y*y*(1-c)+c,    y*z*(1-c)-x*s,  0,
                x*z*(1-c)-y*s,  y*z*(1-c)+x*s,  z*z*(1-c)+c,    0,
                0,              0,              0,              1,
        },
    };
    return ret;
}

struct GLMatrix getGLPerspectiveMatrix(GLfloat fov,GLfloat aspect,GLfloat zNear,GLfloat zFar)
{
    GLfloat f=1/tan(fov/2); // /0 check TODO
    struct GLMatrix ret={
        .data={ f/aspect,   0,  0,                          0,
                0,          f,  0,                          0,
                0,          0,  (zNear+zFar)/(zNear-zFar),  2*zNear*zFar/(zNear-zFar),
                0,          0,  -1,                         0,
        },
    };
    return ret;
}

struct GLMatrix getGLLookAtMatrix(GLfloat eyeX,GLfloat eyeY,GLfloat eyeZ,
                                  GLfloat centerX,GLfloat centerY,GLfloat centerZ,
                                  GLfloat upX,GLfloat upY,GLfloat upZ)
{
    GLfloat f[]={ centerX-eyeX,   centerY-eyeY,   centerZ-eyeZ };
    GLfloat len=sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
    f[0]/=len; f[1]/=len; f[2]/=len;
    GLfloat up[]={ upX,   upY,    upZ };
    len=sqrt(up[0]*up[0]+up[1]*up[1]+up[2]*up[2]);
    up[0]/=len; up[1]/=len; up[2]/=len;
    GLfloat s[]={   f[1]*up[2]-f[2]*up[1],  f[2]*up[0]-f[0]*up[2],  f[0]*up[1]-f[1]*up[0]   };
    GLfloat u[]={   s[1]*f[2]-s[2]*f[1],  s[2]*f[0]-s[0]*f[2],  s[0]*f[1]-s[1]*f[0]   };
    struct GLMatrix ret={
        .data={ s[0],   s[1],   s[2],   0,
                u[0],   u[1],   u[2],   0,
                -f[0],  -f[1],  -f[2],  0,
                0,      0,      0,      1,
        },
    };
    return getGLMatrixProduct(ret,getGLTranslateMatrix(-eyeX,-eyeY,-eyeZ));
}

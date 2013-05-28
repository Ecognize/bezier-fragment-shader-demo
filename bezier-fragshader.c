#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <SDL_config.h>
#include <SDL.h>
#ifdef ANDROID
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "matrix-math.h"
#include "shape.h"

SDL_Event         event;
SDL_Window*      window;
SDL_GLContext glcontext;

GLint linked, loc;
GLuint program, fshader, vshader;

int width, height;

struct camera
{
    double alpha;
    double beta;
    double dist;
} Camera = { M_PI/4, 0.0, 1500.0 };

void motion(int x, int y)
{
    int dx = x, dy = y;

    if (dx < 0) Camera.beta += M_PI * abs(dx) / 64;
    if (dx > 0) Camera.beta -= M_PI * abs(dx) / 64;
    if (dy < 0) Camera.alpha -= M_PI * abs(dy) / 64;
    if (dy > 0) Camera.alpha += M_PI * abs(dy) / 64;
}

GLuint loadShader(char *path,GLenum shaderType)
{
    char buf[4096];


#ifdef ANDROID
    sprintf(buf,"%s",path);
#else
    sprintf(buf,"%s/%s",DATADIR,path);
#endif

    printf("Loading shader source: %s\n",buf);
//     TODO: error handling
    FILE *fp=fopen(buf,"r");
    fseek(fp,0,SEEK_END);
    GLint fsz=(GLint)ftell(fp);
    fseek(fp,0,SEEK_SET);
    GLchar *shText=(char*)malloc(sizeof(GLubyte)*fsz);
    fread(shText,sizeof(GLubyte),(long)fsz,fp);
    fclose(fp);

    GLuint shader=glCreateShader(shaderType);

#ifndef ANDROID
    glShaderSourceARB(shader,1,&shText,&fsz);
    glCompileShaderARB(shader);
#else
    glShaderSource(shader,1,&shText,&fsz);
    glCompileShader(shader);
#endif

    free(shText);

    GLint compiled;

#ifndef ANDROID
    glGetObjectParameterivARB(shader,GL_COMPILE_STATUS,&compiled);
#else
    glGetProgramiv(shader,GL_COMPILE_STATUS,&compiled);
#endif

    if (!compiled)
    {
        GLchar buf[4096];
        GLsizei sz;
        glGetShaderInfoLog(shader,4096,&sz,buf);
        glDeleteShader(shader);
        fprintf(stderr,"Can't compile the shader: %s\n",buf);
        return 0;
    }

    return shader;
}

void camera()
{
    struct GLMatrix proj=getGLPerspectiveMatrix(M_PI/4.0,(GLfloat)width/height,10.0,10000.0);
    proj=getGLMatrixProduct(proj,getGLLookAtMatrix(
        Camera.dist*cos(Camera.alpha)*cos(Camera.beta),Camera.dist*sin(Camera.alpha),Camera.dist*cos(Camera.alpha)*sin(Camera.beta),
        0.0,0.0,0.0,0.0,cos(Camera.alpha)>0?1.0:-1.0,0.0));
    GLint loc=glGetUniformLocation(program,"projMatrix");

#ifndef ANDROID
    glProgramUniformMatrix4fv(program,loc,1,0,proj.data);
#else
    glUniformMatrix4fv(loc,1,0,proj.data);
#endif
}

void draw()
{
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    camera();
    // Perhaps we don't need that recalculated each frame, but meh
    struct GLMatrix model=getGLRotateMatrix(M_PI/2.0,1.0,0.0,0.0);
    model=getGLMatrixProduct(model,getGLTranslateMatrix(-450.0,-550.0,0.0));
    GLint loc=glGetUniformLocation(program,"modelMatrix");

#ifndef ANDROID
    glProgramUniformMatrix4fv(program,loc,1,0,model.data);
#else
    glUniformMatrix4fv(loc,1,0,model.data);
#endif

    glDrawArrays(GL_TRIANGLES,0,vertArraySz*3);
}

void keyb(unsigned char key)
{
    static short fillCutPart = 0, drawFill = 1, drawStroke = 1;
    GLint loc;
    switch (key)
    {
        case SDL_SCANCODE_C: fillCutPart=!fillCutPart;
            loc=glGetUniformLocation(program,"fillCutPart");

            #ifndef ANDROID
                glProgramUniform1i(program,loc,fillCutPart);
            #else
                glUniform1i(loc, fillCutPart);
            #endif

            break;
        case SDL_SCANCODE_F: drawFill=!drawFill;
            loc=glGetUniformLocation(program,"drawFill");

            #ifndef ANDROID
                glProgramUniform1i(program,loc,drawFill);
            #else
                glUniform1i(loc, drawFill);
            #endif

            break;
        case SDL_SCANCODE_S: drawStroke=!drawStroke;
            loc=glGetUniformLocation(program,"drawStroke");

            #ifndef ANDROID
                glProgramUniform1i(program,loc,drawStroke);
            #else
                glUniform1i(loc, drawStroke);
            #endif

            break;
        case SDL_SCANCODE_Z: Camera.dist*= 1.1; break;
        case SDL_SCANCODE_X: Camera.dist*= 0.9; break;
    }
}

void size(int w, int h)
{
    printf("resized! %d, %d\n", w, h);
    width = w; height = h;
    glViewport(0, 0, w, h);
    camera();
}

#ifdef ANDROID
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        perror("can't initialize SDL\n");
        return 1;
    }

    width = 700;
    height = 700;

    window = SDL_CreateWindow("Bezier Fragment Shader Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if(window == NULL)
    {
        fprintf(stderr,"Can't create window: %s\n", SDL_GetError());
        return -1;
    }

    glcontext = SDL_GL_CreateContext(window);

    if(glcontext == NULL)
    {
        fprintf(stderr,"Can't create context: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_MakeCurrent(window, glcontext);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    glViewport(0, 0, width, height);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_DEPTH_TEST);

    fshader = loadShader("bezier.glsl",GL_FRAGMENT_SHADER);
    vshader = loadShader("bezier-vertex.glsl",GL_VERTEX_SHADER);

    if (!(fshader&&vshader))
    {
        fprintf(stderr,"One of shaders failed, aborting.\n");
        return -1;
    }

    program = glCreateProgram();
    glAttachShader(program, fshader);
    glAttachShader(program, vshader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        perror("Can't link the shader\n");
        return -1;
    }

    glUseProgram(program);
    loc = glGetUniformLocation(program, "drawFill");
#ifndef ANDROID
    glProgramUniform1i(program, loc, 1);
#else
    glUniform1i(loc, 1);
#endif

    loc = glGetUniformLocation(program, "drawStroke");

#ifndef ANDROID
    glProgramUniform1i(program, loc, 1);
#else
    glUniform1i(loc, 1);
#endif

#ifndef ANDROID
    glEnableClientState(GL_VERTEX_ARRAY); // Why don't they work like glEnable(A|B) did before? or am I dumb?
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,vertices);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,texcoords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    int running = 1;

    while(running)
    {
        SDL_WaitEvent(&event);

        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    #ifndef ADNROID
                        case SDLK_ESCAPE: running = 0;          break;
                    #else
                        case SDLK_AC_BACK: running = 0;         break;
                    #endif

                    case SDLK_LEFT:   Camera.beta += M_PI / 36; break;
                    case SDLK_RIGHT:  Camera.beta -= M_PI / 36; break;
                    case SDLK_UP:    Camera.alpha += M_PI / 36; break;
                    case SDLK_DOWN:  Camera.alpha -= M_PI / 36; break;
                    default: keyb(event.key.keysym.scancode);   break;
                }
            break;

            case SDL_MOUSEWHEEL:
                Camera.dist*= (event.wheel.y < 0)? 1.1 : 0.9;
            break;

            case SDL_MOUSEMOTION:
                if(event.motion.state == 1) motion(event.motion.xrel, event.motion.yrel);
            break;

            case SDL_WINDOWEVENT_RESIZED:
                size(event.window.data1, event.window.data2);
            break;

            // handle touch events here

            case SDL_QUIT:
                running = 0;
            break;
        }

        draw();
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_MakeCurrent(NULL, NULL);
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

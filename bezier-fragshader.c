#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#ifdef USE_SDL
    #include <SDL_config.h>
    #include <SDL.h>
    #ifdef __ANDROID__
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

#ifdef __ANDROID__
    #include <android/log.h>
    void report(const char* format,...)
    {
        va_list varg;
        va_start(varg,format);
        __android_log_vprint(ANDROID_LOG_DEBUG,"BEZIERS",format,varg);
    }
#else
    void report(const char* format,...)
    {
        va_list varg;
        va_start(varg,format);
        vprintf(format,varg);
    }
#endif

#include "matrix-math.h"
#include "shape.h"

#ifdef USE_SDL
SDL_Event         event;
SDL_Window*      window;
SDL_GLContext glcontext;
#endif

GLint linked;
GLuint program, fshader, vshader;

int width, height;

unsigned frames=0;
unsigned timemark=0;
#ifndef USE_SDL
unsigned currenttime=0;
#define TIMER_RESOLUTION 100
void timer(int value)
{
    currenttime+=TIMER_RESOLUTION;
    glutTimerFunc(TIMER_RESOLUTION,timer,0);
}
#endif

void performanceReport()
{
#ifdef USE_SDL
    unsigned now=SDL_GetTicks();
#else
    unsigned now=currenttime;
#endif
    report("Performance report: %d frames in %d milliseconds.\nAverage fps: %.5f.\n",
           frames,now-timemark,1000.0*(double)(frames)/((double)(now-timemark)));
    timemark=now;
    frames=0;
}


struct camera
{
    double alpha;
    double beta;
    double dist;
} Camera = { M_PI/4, 0.0, 1000.0 };

#ifndef USE_SDL
void mouse(int b,int s,int x,int y)
{
    if (b==3 || b==4)
    {
        Camera.dist*= (b==4)? 1.1 : 0.9;
    }
}
#endif

void motion(int x, int y)
{
#ifndef USE_SDL
    static int mx=-1,my=-1;
    if (mx<0) mx=x; if (my<0) my=y;
    int dx=mx-x,dy=my-y;
    mx=x;my=y;
#else
    int dx = x, dy = y;
#endif

    if (dx < 0) Camera.beta += M_PI * abs(dx) / 64;
    if (dx > 0) Camera.beta -= M_PI * abs(dx) / 64;
    if (dy < 0) Camera.alpha -= M_PI * abs(dy) / 64;
    if (dy > 0) Camera.alpha += M_PI * abs(dy) / 64;
}

GLuint loadShader(char *path,GLenum shaderType)
{
    report("Loading shader source: %s\n",path);
#ifdef __ANDROID__
    SDL_RWops *file = SDL_RWFromFile(path, "r");

    if(file == NULL) report("Can not open file: %s\n", path);
    SDL_RWseek(file,0,RW_SEEK_END);
    GLint fsz=(GLint)SDL_RWtell(file);
    SDL_RWseek(file,0,RW_SEEK_SET);
    char *shText=(char*)malloc(sizeof(char)*fsz);
    SDL_RWread(file, shText, (long)fsz, 1);
    SDL_RWclose(file);
#else
    char buf[4096];
    sprintf(buf,"%s/%s",DATADIR,path);    
//     TODO: error handling
    FILE *fp=fopen(buf,"r");
    if (!fp)
    {
        report("Can not open file: %s\n",buf);
        return 0;
    }
    fseek(fp,0,SEEK_END);
    GLint fsz=(GLint)ftell(fp);
    fseek(fp,0,SEEK_SET);
    char *shText=(char*)malloc(sizeof(char)*fsz);
    fread(shText,sizeof(char),(long)fsz,fp);
    fclose(fp);
#endif
    GLuint shader=glCreateShader(shaderType);

    glShaderSource(shader,1,(const char**)&shText,&fsz);
    glCompileShader(shader);

    free(shText);

    GLint compiled;

    glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);

    if (!compiled)
    {
        GLchar buf[4096];
        GLsizei sz;
        glGetShaderInfoLog(shader,4096,&sz,buf);
        glDeleteShader(shader);
        report("Can't compile the shader: %s\n",buf);
        return 0;
    }

    return shader;
}

GLuint glbuf; 
// WARNING: global scope is evil. never do this at home
int innerPrimitiveSz=0,outerPrimitiveSz=0;
                
void createShapes()
{
    // Primitive separation, done only once
    // Each primitive has 6 points, 2 for coords, 2 for texture coords
    // This may change in future though, for cubic splines
    GLfloat* primitives=(GLfloat *)malloc(sizeof(GLfloat)*vertArraySz*6*2); 
    int i,j;

    for (i=0;i<vertArraySz;i++)
        if (curveClasses[i]<0)
            innerPrimitiveSz++;
    
    outerPrimitiveSz=vertArraySz-innerPrimitiveSz;
    GLfloat *innerPtr=(primitives+innerPrimitiveSz*6*2);
    GLfloat *outerPtr=primitives;
   
    for (i=0;i<vertArraySz;i++)
    {
        GLfloat **ptr = curveClasses[i]<0 ? &innerPtr : &outerPtr;
        GLfloat *arr = *ptr;
        
        for (j=0;j<3;j++)
        {
            arr[j*4]=vertices[i*6+j*2];
            arr[j*4+1]=vertices[i*6+j*2+1];
            
            switch (j)
            {
                case 0: arr[j*4+2]=0.0;
                        arr[j*4+3]=0.0; break;
                case 1: arr[j*4+2]=0.5;
                        arr[j*4+3]=0.0; break;
                case 2: arr[j*4+2]=1.0;
                        arr[j*4+3]=1.0; break;
            }
        }
        *ptr+=12;
    }
    
    /* Putting the data into the buffers for easier access */
    glGenBuffers(1,&glbuf);
    glBindBuffer(GL_ARRAY_BUFFER,glbuf);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*vertArraySz*6*2,primitives,GL_STATIC_DRAW);
    
//     for (i=0;i<vertArraySz*6*2;i++)
//         report("%.3f\t",primitives[i]);
//     report("\n");
    
//     free(primitives);
    /* NB: we're not deleting the buffers. It's a demo anyway */
}

void camera()
{
    struct GLMatrix proj=getGLPerspectiveMatrix(M_PI/4.0,(GLfloat)width/height,10.0,10000.0);
    proj=getGLMatrixProduct(proj,getGLLookAtMatrix(
        Camera.dist*cos(Camera.alpha)*cos(Camera.beta),Camera.dist*sin(Camera.alpha),Camera.dist*cos(Camera.alpha)*sin(Camera.beta),
        0.0,0.0,0.0,0.0,cos(Camera.alpha)>0?1.0:-1.0,0.0));
    glUniformMatrix4fv(glGetUniformLocation(program,"projMatrix"),1,0,proj.data);
}

void draw()
{
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    camera();
    // Perhaps we don't need that recalculated each frame, but meh
    struct GLMatrix model=getGLRotateMatrix(M_PI/2.0,1.0,0.0,0.0);
    model=getGLMatrixProduct(model,getGLRotateMatrix(M_PI/2.0,0.0,0.0,1.0));
    model=getGLMatrixProduct(model,getGLTranslateMatrix(0.0,-150.0,0.0));
//     model=getGLMatrixProduct(model,getGLTranslateMatrix(-450.0,-550.0,0.0));
    glUniformMatrix4fv(glGetUniformLocation(program,"modelMatrix"),1,0,model.data);
    glUniform1i(glGetUniformLocation(program,"drawState"), 1);
    glBindBuffer(GL_ARRAY_BUFFER,glbuf);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*4,0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*4,sizeof(GLfloat)*2);
    glDrawArrays(GL_TRIANGLES,0,innerPrimitiveSz*3);    
   
    glUniform1i(glGetUniformLocation(program,"drawState"), -1);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*4,sizeof(GLfloat)*innerPrimitiveSz*6*2);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*4,sizeof(GLfloat)*(innerPrimitiveSz*6*2+2));
    glDrawArrays(GL_TRIANGLES,0,outerPrimitiveSz*3);
    
#ifndef USE_SDL
    glutSwapBuffers();
#else
    SDL_GL_SwapWindow(window);
#endif
    frames++;
}

#ifndef USE_SDL
void keybs(int key,int mx,int my)
{
    switch (key)
    {
    case GLUT_KEY_LEFT: Camera.beta+=M_PI/36; break;
    case GLUT_KEY_RIGHT: Camera.beta-=M_PI/36; break;
    case GLUT_KEY_UP: Camera.alpha+=M_PI/36; break;
    case GLUT_KEY_DOWN: Camera.alpha-=M_PI/36; break;
    case GLUT_KEY_F1: performanceReport(); break;
    }
}
#endif

#ifndef USE_SDL
void keyb(unsigned char key,int mx,int my)
#else
void keyb(unsigned char key)
#endif
{
    static short fillCutPart = 0, drawFill = 1, drawStroke = 1, useBezier = 1;
    switch (key)
    {
        #ifndef USE_SDL
        case 'C': case 'c':
        #else
        case SDL_SCANCODE_C:
        #endif
            fillCutPart=!fillCutPart;
            glUniform1i(glGetUniformLocation(program,"fillCutPart"), fillCutPart);
            break;
            
        #ifndef USE_SDL
        case 'P': case 'p':
        #else
        case SDL_SCANCODE_P:
        #endif
            useBezier=!useBezier;
            glUniform1i(glGetUniformLocation(program,"useBezier"), useBezier);
            break;
            
        #ifndef USE_SDL
        case 'F': case 'f':
        #else
        case SDL_SCANCODE_F:
        #endif
            drawFill=!drawFill;
            glUniform1i(glGetUniformLocation(program,"drawFill"), drawFill);
            break;
        
        #ifndef USE_SDL
        case 'S': case 's':
        #else
        case SDL_SCANCODE_S:
        #endif
            drawStroke=!drawStroke;
            glUniform1i(glGetUniformLocation(program,"drawStroke"), drawStroke);
            break;
                
        #ifndef USE_SDL
        case 'Z': case 'z':
        #else
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_VOLUMEDOWN:
        #endif
            Camera.dist*= 1.1; break;
        
        #ifndef USE_SDL
        case 'X': case 'x':
        #else
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_VOLUMEUP:
        #endif
            Camera.dist*= 0.9; break;
        #ifndef USE_SDL
        case 27: // Escape
            performanceReport();
            exit(0);
            break;
        #endif
    }
}

void size(int w, int h)
{
    report("Window resized: %d, %d\n", w, h);
    width = w; height = h;
    glViewport(0, 0, w, h);
}


#ifdef __ANDROID__
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
#ifdef USE_SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        report("Can't initialize SDL\n");
        return 1;
    }
// TODO fullscreen + get screen size
#ifndef __ANDROID__
    width=height=700;
#else
    width=320;
    height=480;
//     const SDL_VideoInfo* vinfo=SDL_GetVideoInfo();    
//     width = vinfo->current_w;
//     height = vinfo->current_h;
//     report("Detected %dx%d resolution.\n",width,height);
#endif

    window = SDL_CreateWindow("Bezier Fragment Shader Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if(window == NULL)
    {
        report("Can't create window: %s\n", SDL_GetError());
        return -1;
    }

    glcontext = SDL_GL_CreateContext(window);

    if(glcontext == NULL)
    {
        report("Can't create context: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_MakeCurrent(window, glcontext);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#else
    glutInit(&argc,argv);
    glutInitWindowSize(700,700);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
    glutCreateWindow("Bezier Fragment Shader Demo");
#endif
    

    glViewport(0, 0, width, height);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_DEPTH_TEST);

    fshader = loadShader("bezier.glsl",GL_FRAGMENT_SHADER);
    vshader = loadShader("bezier-vertex.glsl",GL_VERTEX_SHADER);

    if (!(fshader&&vshader))
    {
        report("One of shaders failed, aborting.\n");
        return -1;
    }

    program = glCreateProgram();
    glAttachShader(program, fshader);
    glAttachShader(program, vshader);
    glBindAttribLocation(program, 0, "vertexPos");
    glBindAttribLocation(program, 1, "bezCoordsAttr");
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        report("Can't link the shader\n");
        return -1;
    }

    glUseProgram(program);

    glUniform1i(glGetUniformLocation(program, "drawFill"), 1);
    glUniform1i(glGetUniformLocation(program, "useBezier"), 1);
    glUniform1i(glGetUniformLocation(program, "drawStroke"), 1);

#ifndef __ANDROID__
//     glEnableClientState(GL_VERTEX_ARRAY); // Why don't they work like glEnable(A|B) did before? or am I dumb?
//     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    createShapes();
    
#ifdef USE_SDL
    int running = 1;
    timemark=SDL_GetTicks();

    while(running)
    {
        if ( SDL_PollEvent(&event) > 0 )
        {
//         SDL_WaitEvent(&event);

            switch(event.type)
            {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        #ifdef __ANDROID__
                        case SDLK_AC_SEARCH:
                        #endif  
                        case SDLK_F1: performanceReport(); break;
                        
                        #ifndef __ANDROID__
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

                // Note, the first frame flickers, TODO workaround
                // TODO: track the real sequence of WINDOWEVENT_ENTER and WINDOWEVENT_SIZE_CHANGED events 
                case SDL_WINDOWEVENT:
                    if (event.window.event==SDL_WINDOWEVENT_SIZE_CHANGED)
                        size(event.window.data1, event.window.data2);
                break;

                // handle touch events here

                case SDL_QUIT:
                    running = 0;
                break;
            }
        }

        draw();
    }
    
    performanceReport();
    
    SDL_GL_MakeCurrent(NULL, NULL);
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
#else
    glutReshapeFunc(size);
    glutDisplayFunc(draw);
    glutIdleFunc(draw);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSpecialFunc(keybs);
    glutKeyboardFunc(keyb);
    glutTimerFunc(TIMER_RESOLUTION,timer,0);
    glutMainLoop();
#endif
    
    return 0;
}

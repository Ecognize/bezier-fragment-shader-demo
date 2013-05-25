#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <SDL_config.h>
#include <SDL.h>
#ifdef GLES
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

SDL_Event         event;
SDL_Window*      window;
SDL_GLContext glcontext;

GLint linked, loc;
GLuint program, shader;

int width, height;

struct camera
{
    double alpha;
    double beta;
    double dist;
} Camera = { M_PI/4,0.0,1500.0 };

void motion(int x, int y)
{
    static int mx=-1,my=-1;
    if (mx<0) mx=x; if (my<0) my=y;
    int dx=mx-x,dy=my-y;
    if (dx<0) Camera.beta+=M_PI*abs(dx)/64;
    if (dx>0) Camera.beta-=M_PI*abs(dx)/64;
    if (dy<0) Camera.alpha-=M_PI*abs(dy)/64;
    if (dy>0) Camera.alpha+=M_PI*abs(dy)/64;
}

GLuint loadShader(char *path)
{
    char buf[4096];
    sprintf(buf,"%s/%s",DATADIR,path);
    printf("Loading shader source: %s\n",buf);
//     TODO: error handling
    FILE *fp=fopen(buf,"r");
    fseek(fp,0,SEEK_END);
    GLint fsz=(GLint)ftell(fp);
    fseek(fp,0,SEEK_SET);
    GLubyte *shText=(char*)malloc(sizeof(GLubyte)*fsz);
    fread(shText,sizeof(GLubyte),(long)fsz,fp);
    fclose(fp);

    GLuint shader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSourceARB(shader,1,&shText,&fsz);
    glCompileShaderARB(shader);
    free(shText);

    GLint compiled;
    glGetObjectParameterivARB(shader,GL_COMPILE_STATUS,&compiled);
    if (!compiled)
    {
        perror("Can't compile the shader\n");
        return 0;
    }

    return shader;
}

void camera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0,(float)width/height,10.0,10000.0);
    gluLookAt(Camera.dist*cos(Camera.alpha)*cos(Camera.beta),Camera.dist*sin(Camera.alpha),Camera.dist*cos(Camera.alpha)*sin(Camera.beta),0.0,0.0,0.0,0.0,cos(Camera.alpha)>0?1.0:-1.0,0.0);
    glMatrixMode(GL_MODELVIEW);
}

void draw()
{
    static GLfloat vertices[] = {
    477,    -58,    479,    -54,    504.5,  -18.5,
    504.5,  -18.5,  530,    17, 534,    40,
    534,    40, 538,    79, 545,    77,
    545,    77, 548,    77, 550,    65,
    550,    65, 553,    54, 551,    32.5,
    551,    32.5,   549,    11, 552,    3,
    552,    3,  581,    30, 586,    75.5,
    586,    75.5,   591,    121,    576,    157,
    576,    157,    554,    215,    504,    256.5,
    504,    256.5,  454,    298,    390,    318,
    390,    318,    359,    340,    371,    416,
    371,    416,    375,    433,    377.5,  441,
    377.5,  441,    380,    449,    386,    462.5,
    386,    462.5,  392,    476,    400.5,  484.5,
    400.5,  484.5,  409,    493,    421,    498,
    421,    498,    430,    504,    461,    520,
    461,    520,    492,    536,    507,    547,
    507,    547,    499,    549,    486,    556.5,
    486,    556.5,  473,    564,    466,    566,
    466,    566,    459,    568,    443,    563,
    443,    563,    361,    540,    288,    451,
    288,    451,    220,    337,    225,    251,
    225,    251,    225,    190,    264,    138.5,
    264,    138.5,  303,    87, 361,    78,
    361,    78, 413,    68, 467,    100,
    467,    100,    476,    106,    480.5,  108.5,
    480.5,  108.5,  485,    111,    494,    114.5,
    494,    114.5,  503,    118,    508.5,  117.5,
    508.5,  117.5,  514,    117,    520,    112.5,
    520,    112.5,  526,    108,    529,    98,
    529,    98, 462,    47, 410,    25,
    410,    25, 368,    10, 321.5,  18,
    321.5,  18, 275,    26, 242,    56,
    242,    56, 209,    82, 188.5,  118,
    188.5,  118,    168,    154,    167,    194,
    167,    194,    162,    271,    182.5,  343.5,
    182.5,  343.5,  203,    416,    252,    470,
    252,    470,    326,    558,    446,    583,
    446,    583,    444,    605,    443,    615,
    443,    615,    442,    625,    447.5,  641,
    447.5,  641,    453,    657,    467,    665,
    467,    665,    481,    672,    499,    672.5,
    499,    672.5,  517,    673,    531,    662,
    531,    662,    545,    651,    542,    633,
    542,    633,    528,    625,    510,    628,
    510,    628,    492,    631,    476.5,  624.5,
    476.5,  624.5,  461,    618,    461,    596,
    461,    596,    464,    584,    476,    579.5,
    476,    579.5,  488,    575,    510,    574,
    510,    574,    532,    573,    536,    572,
    536,    572,    634,    572,    687,    519,
    687,    519,    732,    466,    726,    357,
    726,    357,    720,    301,    690,    250.5,
    690,    250.5,  660,    200,    614,    166,
    614,    166,    606,    157,    608.5,  147.5,
    608.5,  147.5,  611,    138,    615,    121.5,
    615,    121.5,  619,    105,    616,    96,
    616,    96, 619,    37, 575,    -3,
    575,    -3, 551,    -21,    540,    -28.5,
    540,    -28.5,  529,    -36,    511.5,  -45,
    511.5,  -45,    494,    -54,    477,    -58,
    635,    294,    659,    338,    666,    385.5,
    666,    385.5,  673,    433,    659,    481,
    659,    481,    644,    518,    606.5,  536,
    606.5,  536,    569,    554,    536,    535,
    536,    535,    532,    533,    516.5,  525.5,
    516.5,  525.5,  501,    518,    495.5,  515,
    495.5,  515,    490,    512,    476,    504.5,
    476,    504.5,  462,    497,    455.5,  492.5,
    455.5,  492.5,  449,    488,    438.5,  479.5,
    438.5,  479.5,  428,    471,    421.5,  463.5,
    421.5,  463.5,  415,    456,    408,    446.5,
    408,    446.5,  401,    437,    397,    426,
    397,    426,    383,    405,    389,    380,
    389,    380,    395,    355,    419,    344,
    419,    344,    523,    278,    572,    224,
    572,    224,    578,    218,    601.5,  246.5,
    601.5,  246.5,  625,    275,    635,    294,
    449,    544,    455,    553,    465,    549,
    465,    549,    473,    545,    469,    542,
    469,    542,    466,    540,    456,    541,
    456,    541,    449,    543,    449,    544,
    };
    static GLfloat texcoords[] = {
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    0.0,0.0,0.5,    0.0,    1.0,1.0,
    };
    static int vertArraySz=82;


    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    camera();
    glPushMatrix();
        glRotatef(90.0,1.0,0.0,0.0);
        glTranslatef(-450.0,-550.0,0.0);
        glColor3f(1.0,1.0,1.0);
        glVertexPointer(2,GL_FLOAT,0,vertices);
        glTexCoordPointer(2,GL_FLOAT,0,texcoords);
        glDrawArrays(GL_TRIANGLES,0,vertArraySz*3);
    glPopMatrix();
}

void keyb(unsigned char key,int mx,int my)
{
    static short fillCutPart = 0, drawFill = 1, drawStroke = 1;
    GLint loc;
    switch (key)
    {
        case SDL_SCANCODE_C: fillCutPart=!fillCutPart;
            loc=glGetUniformLocation(program,"fillCutPart");
            glProgramUniform1i(program,loc,fillCutPart);
            break;
        case SDL_SCANCODE_F: drawFill=!drawFill;
            loc=glGetUniformLocation(program,"drawFill");
            glProgramUniform1i(program,loc,drawFill);
            break;
        case SDL_SCANCODE_S: drawStroke=!drawStroke;
            loc=glGetUniformLocation(program,"drawStroke");
            glProgramUniform1i(program,loc,drawStroke);
            break;
        case SDL_SCANCODE_Z: Camera.dist*= 1.1; break;
        case SDL_SCANCODE_X: Camera.dist*= 0.9; break;
    }
}

void size(int w, int h)
{
    width = w; height = h;
    glViewport(0, 0, w, h);
    camera();
}

int main(int argc, char *argv[])
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
    glShadeModel(GL_SMOOTH);

    shader = loadShader("bezier.glsl");

    if (! shader)
    {
        fprintf(stderr,"Can't create context: %s\n", SDL_GetError());
        return -1;
    }

    program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        perror("Can't link the shader\n");
        return -1;
    }

    glUseProgram(program);
    loc = glGetUniformLocation(program, "drawFill");
    glProgramUniform1i(program, loc, 1);
    loc = glGetUniformLocation(program, "drawStroke");
    glProgramUniform1i(program, loc, 1);

    glEnableClientState(GL_VERTEX_ARRAY); // Why don't they work like glEnable(A|B) did before? or am I dumb?
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    int running = 1;

    while(running)
    {
        SDL_WaitEvent(&event);

        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: running = 0;              break;
                    case SDLK_LEFT:   Camera.beta += M_PI / 36; break;
                    case SDLK_RIGHT:  Camera.beta -= M_PI / 36; break;
                    case SDLK_UP:    Camera.alpha += M_PI / 36; break;
                    case SDLK_DOWN:  Camera.alpha -= M_PI / 36; break;
                    default: keyb(event.key.keysym.scancode,0,0);   break; // WARNING: passing zeroes
                }
            break;

            case SDL_MOUSEWHEEL:
                Camera.dist*= (event.wheel.y < 0)? 1.1 : 0.9;
            break;

            case SDL_MOUSEMOTION:
                motion(event.motion.x, event.motion.y);
            break;

            case SDL_WINDOWEVENT_RESIZED:
                size(event.window.data1, event.window.data2);
            break;

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

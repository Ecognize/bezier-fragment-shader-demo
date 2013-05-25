#ifdef _WIN32
    #include <windows.h> 
#endif
#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

unsigned dlist=0;
struct camera
{
    double alpha;
    double beta;
    double dist;
} Camera = { M_PI/4,0.0,1500.0 };
int width; int height;

void draw();

void mouse(int b,int s,int x,int y)
{
    if (b==3 || b==4)
    {
        Camera.dist*= (b==4)? 1.1 : 0.9;
        draw();
    }
}

void motion(int x,int y)
{
    static int mx=-1,my=-1;
    if (mx<0) mx=x; if (my<0) my=y;
    int dx=mx-x,dy=my-y;
    if (dx<0) Camera.beta+=M_PI*abs(dx)/64;
    if (dx>0) Camera.beta-=M_PI*abs(dx)/64;
    if (dy<0) Camera.alpha-=M_PI*abs(dy)/64;
    if (dy>0) Camera.alpha+=M_PI*abs(dy)/64;
    mx=x;my=y;
    draw();
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
    504.5,  -18.5,  530,    17, 506.5,  0.5,     
    506.5,  0.5,    481,    -19,    488,    -21,     
    488,    -21,    480,    -58,    482,    -70,     
    482,    -70,    480,    -69,    478,    -90.5,   
    478,    -90.5,  476,    -112,   478,    -87.5,   
    478,    -87.5,  506,    -31,    511,    14.5,    
    511,    14.5,   516,    60, 467,    23.5,    
    467,    23.5,   455,    0,  405,    41.5,    
    405,    41.5,   355,    83, 363,    3.5,     
    363,    3.5,    446,    -36,    458,    40,  
    458,    40, 481,    -41,    483.5,  -33,     
    483.5,  -33,    486,    -25,    485.5,  -36.5,   
    485.5,  -36.5,  485,    -48,    491.5,  -36,     
    491.5,  -36,    498,    -24,    497.5,  -44.5,   
    497.5,  -44.5,  486,    -52,    517,    -36,     
    517,    -36,    548,    -20,    523,    -31,     
    523,    -31,    469,    -56,    456,    -48.5,   
    456,    -48.5,  443,    -41,    457,    -48.5,   
    457,    -48.5,  471,    -56,    454,    -61,     
    454,    -61,    395,    -81,    322,    -170,    
    322,    -170,   409,    -172,   414,    -258,    
    414,    -258,   477,    -119,   516,    -170.5,  
    516,    -170.5, 555,    -222,   574,    -118.5,  
    574,    -118.5, 529,    -68,    583,    -36,     
    583,    -36,    486,    -52,    490.5,  -49.5,   
    490.5,  -49.5,  495,    -47,    490.5,  -52,     
    490.5,  -52,    486,    -57,    491.5,  -55,     
    491.5,  -55,    497,    -53,    488.5,  -63,     
    488.5,  -63,    480,    -73,    486,    -72.5,   
    486,    -72.5,  410,    -109,   358,    -131,    
    358,    -131,   435,    -73,    388.5,  -65,     
    388.5,  -65,    342,    -57,    397.5,  -20,     
    397.5,  -20,    444,    -32,    423.5,  4,   
    423.5,  4,  403,    40, 455.5,  18,  
    455.5,  18, 472,    19, 492.5,  91.5,    
    492.5,  91.5,   513,    164,    546.5,  68.5,    
    546.5,  68.5,   551,    30, 671,    55,  
    671,    55, 475,    -36,    474,    -26,     
    474,    -26,    473,    -16,    481.5,  -32,     
    481.5,  -32,    490,    -48,    496.5,  -34,     
    496.5,  -34,    491,    -51,    509,    -50.5,   
    509,    -50.5,  527,    -50,    509,    -68.5,   
    509,    -68.5,  491,    -87,    488,    -87,     
    488,    -87,    463,    -66,    445,    -63,     
    445,    -63,    427,    -60,    443.5,  -61.5,   
    443.5,  -61.5,  460,    -63,    461.5,  -86.5,   
    461.5,  -86.5,  480,    -70,    492,    -74.5,   
    492,    -74.5,  504,    -79,    511,    -63.5,   
    511,    -63.5,  518,    -48,    503,    -60,     
    503,    -60,    575,    -58,    628,    -111,    
    628,    -111,   522,    -111,   516,    -220,    
    516,    -220,   471,    -114,   441,    -164.5,  
    441,    -164.5, 411,    -215,   401,    -142.5,  
    401,    -142.5, 469,    -67,    471.5,  -76.5,   
    471.5,  -76.5,  474,    -86,    483.5,  -84,     
    483.5,  -84,    493,    -82,    478,    -83.5,   
    478,    -83.5,  480,    -117,   436,    -157,    
    436,    -157,   453,    -76,    442,    -83.5,   
    442,    -83.5,  431,    -91,    448.5,  -74.5,   
    448.5,  -74.5,  466,    -58,    442.5,  -71,     
    635,    294,    659,    338,    666,    385.5,   
    666,    385.5,  673,    433,    628,    389.5,   
    628,    389.5,  620,    331,    582.5,  349,     
    582.5,  349,    545,    367,    564.5,  293,     
    564.5,  293,    631,    292,    615.5,  284.5,   
    615.5,  284.5,  600,    277,    614,    283.5,   
    614,    283.5,  628,    290,    615.5,  283.5,   
    615.5,  283.5,  603,    277,    614.5,  282,     
    614.5,  282,    626,    287,    618,    281,     
    618,    281,    610,    275,    618,    278,     
    618,    278,    626,    281,    621.5,  277,     
    621.5,  277,    617,    273,    624,    273.5,   
    624,    273.5,  621,    273,    627,    248,     
    627,    248,    633,    223,    665,    258,     
    665,    258,    739,    228,    788,    174,     
    788,    174,    641,    288,    664.5,  316.5,   
    664.5,  316.5,  688,    345,    668.5,  341.5,   
    449,    544,    455,    553,    465,    549,     
    465,    549,    457,    540,    453,    537,     
    453,    537,    446,    542,    436,    543,     
    436,    543,    442,    546,    442,    547,     
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
        glTranslatef(-1000.0,200.0,0.0);
        glScalef(2.0,2.0,2.0);
        glColor3f(1.0,1.0,1.0);
        glVertexPointer(2,GL_FLOAT,0,vertices);
        glTexCoordPointer(2,GL_FLOAT,0,texcoords);
        glDrawArrays(GL_TRIANGLES,0,vertArraySz);
    glPopMatrix();
    glutSwapBuffers();
}

void keybs(int key,int mx,int my)
{
    switch (key)
    {
    case GLUT_KEY_LEFT: Camera.beta+=M_PI/36; break;
    case GLUT_KEY_RIGHT: Camera.beta-=M_PI/36; break;
    case GLUT_KEY_UP: Camera.alpha+=M_PI/36; break;
    case GLUT_KEY_DOWN: Camera.alpha-=M_PI/36; break;
    case GLUT_KEY_F1: exit(0); break;
    }
    draw();
}

GLuint program;

void keyb(unsigned char key,int mx,int my)
{
    static short fillCutPart=0,drawFill=1,drawStroke=1;
    GLint loc;
//     static short wireframe=0,normals=0;
    switch (key)
    {
        case 'c': case 'C': fillCutPart=!fillCutPart; 
            loc=glGetUniformLocation(program,"fillCutPart");
            glProgramUniform1i(program,loc,fillCutPart);
            break;
        case 'f': case 'F': drawFill=!drawFill; 
            loc=glGetUniformLocation(program,"drawFill");
            glProgramUniform1i(program,loc,drawFill);
            break;
        case 's': case 'S': drawStroke=!drawStroke; 
            loc=glGetUniformLocation(program,"drawStroke");
            glProgramUniform1i(program,loc,drawStroke);
            break;
//     case 'w': case 'W': wireframe=!wireframe; create_lists(wireframe,normals); break;
//     case 'n': case 'N': normals=!normals; create_lists(wireframe,normals); break;
        case '-': Camera.dist*= 1.1; break;
        case '+': Camera.dist*= 0.9; break;
    }
    draw();
}

void size(int w,int h)
{
    width=w; height=h;
    glViewport(0,0,w,h);
    camera();
}

int main(int argc,char *argv[])
{
    glutInit(&argc,argv);
    glutInitWindowSize(700,700);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
    glutCreateWindow("Bezier Fragment Shader Demo");
    glClearColor(0.0,0.0,0.0,0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
 
    GLuint shader=loadShader("bezier.glsl");
    if (shader==0)
        return -1;
    program=glCreateProgram();
    glAttachShader(program,shader);
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program,GL_LINK_STATUS,&linked);
    if (!linked)
    {
        perror("Can't link the shader\n");
        return -1;
    }
    glUseProgram(program);
    GLint loc=glGetUniformLocation(program,"drawFill");
    glProgramUniform1i(program,loc,1);
    loc=glGetUniformLocation(program,"drawStroke");
    glProgramUniform1i(program,loc,1);
    
    glEnableClientState(GL_VERTEX_ARRAY); // Why don't they work like glEnable(A|B) did before? or am I dumb?
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glutReshapeFunc(size);
    glutDisplayFunc(draw);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSpecialFunc(keybs);
    glutKeyboardFunc(keyb);
    glutMainLoop();
    return 0;
}

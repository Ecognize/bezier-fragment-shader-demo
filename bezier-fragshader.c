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
    static GLfloat vertices[] = 
        { 1.0,0.0,0.0, 0.0,0.0,1.0, 0.0,1.0,0.0 };
    static GLfloat texcoords[] =
        { 0.0,0.0, 0.5,0.0, 1.0,1.0 };
    
    
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);   
    camera();
    glPushMatrix();
        glTranslatef(0.0,-50.0,0.0);
        glScalef(150.0,150.0,150.0);
        glColor3f(1.0,1.0,1.0);
        glVertexPointer(3,GL_FLOAT,0,vertices);
        glTexCoordPointer(2,GL_FLOAT,0,texcoords);
        glDrawArrays(GL_TRIANGLES,0,3);
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

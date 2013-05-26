uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(void)
{
    gl_Position=projMatrix*modelMatrix*gl_Vertex;
}
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(void)
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position=projMatrix*modelMatrix*gl_Vertex;
}
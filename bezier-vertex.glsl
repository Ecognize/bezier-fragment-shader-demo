attribute vec4 vertexPos;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(void)
{
    switch (gl_VertexID%3)
    {
        case 0: gl_TexCoord[0] = vec4(0.0,0.0,0.0,0.0); break;
        case 1: gl_TexCoord[0] = vec4(0.5,0.0,0.0,0.0); break;
        case 2: gl_TexCoord[0] = vec4(1.0,1.0,0.0,0.0); break;
    }   
    gl_Position=projMatrix*modelMatrix*vertexPos;
}
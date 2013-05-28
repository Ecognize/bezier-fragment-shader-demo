attribute vec4 vertexPos;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(void)
{
    int vertId=gl_VertexID%3;
    if (vertId==0)
    {
        gl_TexCoord[0] = vec4(0.0,0.0,0.0,0.0);
    }
    else if (vertId==1)
    {
        gl_TexCoord[0] = vec4(0.5,0.0,0.0,0.0);
    }
    else
    {
        gl_TexCoord[0] = vec4(1.0,1.0,0.0,0.0);
    }   
    gl_Position=projMatrix*modelMatrix*vertexPos;
}

attribute vec4 vertex_pos;
attribute vec4 vertex_tcord;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(void)
{
    gl_TexCoord[0] = vertex_tcord;
    gl_Position=projMatrix*modelMatrix*vertex_pos;
}
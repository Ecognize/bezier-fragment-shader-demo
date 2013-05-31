attribute vec4 vertexPos;
attribute vec4 bezCoordsAttr;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
varying highp vec4 bezCoords;

void main(void)
{
    bezCoords.stpq=bezCoordsAttr;
    gl_Position=projMatrix*modelMatrix*vertexPos;
}

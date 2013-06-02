attribute vec4 vertexPos;
attribute vec4 bezCoordsAttr;
uniform mat4 projModelMatrix;
varying highp vec4 bezCoords;

void main(void)
{
    bezCoords.stpq=bezCoordsAttr;
    gl_Position=projModelMatrix*vertexPos;
}

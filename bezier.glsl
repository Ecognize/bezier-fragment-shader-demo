uniform bool drawFill;
uniform bool drawStroke;
uniform bool fillCutPart;
uniform bool useBezier;
uniform int drawState;
varying highp vec4 bezCoords;

void main (void)  
{
    if (useBezier || drawState==0)
    {
        highp float dist=pow(bezCoords.s,2.0)-bezCoords.t;
        // Make use of GreaterThan etc
        if (drawStroke && (abs(dist)<0.01))
        {
            gl_FragColor = vec4(1.0,1.0,1.0,1.0);  
        }
        else if ((drawState<0 && dist>0.0) || (drawState>0 && dist<0.0))
        {
            if (fillCutPart)
                gl_FragColor = vec4(0.4,0.4,1.0,1.0);
            else
                discard;
        }
        else
        {
            if (drawFill)
                gl_FragColor = vec4(0.65,0.15,0.15,1.0); 
            else
                discard;
        }
    }
    else
        gl_FragColor = vec4(0.6,0.8,0.7,1.0);
}

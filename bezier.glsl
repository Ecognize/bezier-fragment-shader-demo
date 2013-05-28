uniform bool drawFill;
uniform bool drawStroke;
uniform bool fillCutPart;
uniform bool useBezier;
uniform int drawState;

void main (void)  
{
    if (useBezier || drawState==0)
    {
        float dist=pow(gl_TexCoord[0].s,2)-gl_TexCoord[0].t;
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

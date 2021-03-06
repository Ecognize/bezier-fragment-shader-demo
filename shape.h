#ifndef SHAPE_H_
#define SHAPE_H_
static GLfloat vertices[] = {
// Outer heart right half
0.0,0.0, 20.0,70.0, 70.0,100.0,
70.0,100.0, 140.0,130.0, 150.0,200.0,
150.0,200.0, 135.0,280.0, 75.0,290.0,
75.0,290.0, 20.0,280.0, 0.0,220.0,
// Outer heart left half
0.0,220.0, -20.0,280.0, -75.0,290.0,
-75.0,290.0, -135.0,280.0, -150.0,200.0,
-150.0,200.0, -140.0,130.0, -70.0,100.0,
-70.0,100.0, -20.0,70.0, 0.0, 0.0,
// Inner heart left half
0.0,35.0, 15.0,90.0, 55.0,105.0,
55.0,105.0, 105.0,135.0, 115.0,200.0,
115.0,200.0, 100.0,245.0, 60.0,255.0,
60.0,255.0, 15.0,245.0, 0.0,200.0,
// Inner heart right half
0.0,200.0, -15.0,245.0, -60.0,255.0,
-60.0,255.0, -100.0,245.0, -115.0,200.0,
-115.0,200.0, -105.0,135.0, -55.0,105.0,
-55.0,105.0, -15.0,90.0, 0.0,35.0,
};

static GLfloat curveClasses[]=
{
// Outer heart right half
1,-1,-1,-1,
// Outer heart left half 
-1,-1,-1,1,
// Inner heart left half
-1,1,1,1,
// Inner heart right half
1,1,1,-1,
};

static int vertArraySz=16;
#endif

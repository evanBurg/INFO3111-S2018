#version 420
// If 420 doesn't work, try 330 

attribute vec3 vCol;		// float r, g, b;
attribute vec3 vPos;		// float x, y, z;

uniform mat4 MVP;
uniform vec3 meshColour; 

varying vec3 color;

void main()
{
    vec3 newVertex = vPos;				
    gl_Position = MVP * vec4(newVertex, 1.0);
    color = meshColour;				// color = vCol;
    color *= 0.01f;
    color += vCol;
};

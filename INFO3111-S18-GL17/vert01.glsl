#version 420
// If 420 doesn't work, try 330 
// The VERTEX SHADER

attribute vec3 vCol;		// float r, g, b;
attribute vec3 vPos;		// float x, y, z;

//uniform mat4 MVP;		// Model View Projection
uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform vec3 meshColour; 

// glUniform1f()  pass in 0 or 1 
uniform bool bUseVertexColour;		// 0 or 1 Really a float

varying vec3 color;

void main()
{
    vec3 newVertex = vPos;				
	
	// Check the main render to see a similar line of code
	mat4 matMVP = matProjection * matView * matModel;
	gl_Position = matMVP * vec4(newVertex, 1.0);
	
//    gl_Position = MVP * vec4(newVertex, 1.0);
	
	color = meshColour;	
	
	if ( bUseVertexColour )
	{
		color = vCol;
	}
};

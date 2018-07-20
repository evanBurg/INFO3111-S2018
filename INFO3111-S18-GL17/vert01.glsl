#version 420
// If 420 doesn't work, try 330 
// The VERTEX SHADER

attribute vec3 vCol;		// float r, g, b;
attribute vec3 vPos;		// float x, y, z;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

//uniform mat4 MVP;
uniform vec3 meshColour; 

// glUniform1f()  pass in 0 or 1 
uniform bool bUseVertexColour;		// 0 or 1 Really a float

out vec3 color;
out vec3 vertInWorld;
out mat4 mat4WorldRotOnly;

void main()
{
    vec3 newVertex = vPos;				
	mat4 matMVP = matProjection * matView * matModel;
    gl_Position = matMVP * vec4(newVertex, 1.0);
	
	vec4 vertInWorldTemp = matModel * vec4(newVertex, 1.0);
	vertInWorld = vec3(vertInWorldTemp.xyz);
	
	//For diffuse, just copy for now
	mat4WorldRotOnly = matModel;
	
	color = meshColour;	
	
	if ( bUseVertexColour )
	{
		color = vCol;
	}
};

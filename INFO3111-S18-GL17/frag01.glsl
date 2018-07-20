#version 420
// If 420 doesn't work, try 330 
// After 3.x, it is the version, so 4.20
// Also, the version of GLSL WASN'T 
// the same as the OpenGL version.

// FRAGMENT SHADER

in vec3 color;				// was varying
in vec3 vertInWorld;		// was varying
in mat4 mat4WorldRotOnly;	// To be used on Tuesday


// Add these to the FRAGMENT shader
// (From the day 5 lesson plan word document)
uniform vec3 lightPosition;
uniform vec4 lightDirection;
uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;
uniform vec4 lightSpecular; 		// rgb = colour, w = intensity
// x = constant, y = linear, z = quadratic, w = “type”
uniform vec4 lightAttenAndType;		

// We'll start up in about 10-15 minutes
// (around 1:45-ish)

void main()
{
	// Take the colour of the thing, and set the output 
	//	colour to that to start... 
	vec3 outDiffuse = color; 
	
	// Calculate the distance between the light 
	// and the vertex that this fragment is using
	float lightDistance = distance( vertInWorld, lightPosition );
	lightDistance = abs(lightDistance);

	float attenuation = 1.0f / 
        ( lightAttenAndType.x 						// 0  
        + lightAttenAndType.y * lightDistance					// Linear
        + lightAttenAndType.z * lightDistance * lightDistance );	// Quad
	outDiffuse.rgb *= attenuation;

    gl_FragColor = vec4(outDiffuse, 1.0);
};






//	gl_FragColor.rgb *= 0.0001f;		// Almost zero	
//	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );

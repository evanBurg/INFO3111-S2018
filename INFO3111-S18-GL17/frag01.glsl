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
uniform vec3 lightDirection;
uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;
uniform vec4 lightSpecular; 		// xyz = colour, w = intensity
uniform vec4 lightAttenAndType;		// x = constant, y = linear, z = quadratic, w = “type”

void main()
{
	vec3 outDiffuse = color;
	
	float lightDistance = abs(distance(vertInWorld, lightPosition));

	
	float attenuation = 1.0f / (lightAttenAndType.x + lightAttenAndType.y * lightDistance + lightAttenAndType.z * lightDistance * lightDistance);
	outDiffuse.rgb *= attenuation;
	
    gl_FragColor = vec4(outDiffuse, 1.0);
	
	
	
	
//	gl_FragColor.rgb *= 0.0001f;		// Almost zero
	
//	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );
};

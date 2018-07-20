#version 420
// If 420 doesn't work, try 330 
// After 3.x, it is the version, so 4.20
// Also, the version of GLSL WASN'T 
// the same as the OpenGL version.

in vec3 color;
in vec3 vertInWorld;

// You can keep these here.
// (They aren't doing anything, yet)
uniform vec3 LightPosition;
uniform vec3 LightDirection;
uniform vec3 LightDiffuse;		// ...
uniform vec3 LightAmbient;
uniform vec3 LightSpecular;
uniform vec3 LightAttenAndType;

void main()
{
	
    gl_FragColor = vec4(color, 1.0);
	
	
	
	
//	gl_FragColor.rgb *= 0.0001f;		// Almost zero
	
//	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );
};

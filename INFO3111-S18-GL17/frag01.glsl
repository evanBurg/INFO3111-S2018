#version 420
// If 420 doesn't work, try 330 
// After 3.x, it is the version, so 4.20
// Also, the version of GLSL WASN'T 
// the same as the OpenGL version.

// FRAGMENT SHADER

in vec3 color;				// was varying
in vec3 vertInWorld;		// was varying
in mat4 mat4WorldRotOnly;	// To be used on Tuesday


// LIGHT 1
uniform vec3 lightPosition1;
uniform vec4 lightDirection1;
uniform vec4 lightDiffuse1;
uniform vec4 lightAmbient1;
uniform vec4 lightSpecular1;
uniform vec4 lightAttenAndType1;		
uniform vec3 lightColour1;
//

// LIGHT 2
uniform vec3 lightPosition2;
uniform vec4 lightDirection2;
uniform vec4 lightDiffuse2;
uniform vec4 lightAmbient2;
uniform vec4 lightSpecular2;
uniform vec4 lightAttenAndType2;		
uniform vec3 lightColour2;
//

// LIGHT 3
uniform vec3 lightPosition3;
uniform vec4 lightDirection3;
uniform vec4 lightDiffuse3;
uniform vec4 lightAmbient3;
uniform vec4 lightSpecular3;
uniform vec4 lightAttenAndType3;		
uniform vec3 lightColour3;
//

uniform bool AffectedByLight;
void main()
{
	// Take the colour of the thing, and set the output 
	//	colour to that to start... 
	vec3 outDiffuse = vec3(0.0f, 0.0f, 0.0f); 
	if(AffectedByLight){
		// Calculate the distance between the light 
		// and the vertex that this fragment is using
		float lightDistance1 = abs(distance( vertInWorld, lightPosition1));
		float lightDistance2 = abs(distance( vertInWorld, lightPosition2));
		float lightDistance3 = abs(distance( vertInWorld, lightPosition3));

		float attenuation1 = 1.0f / ( lightAttenAndType1.x + lightAttenAndType1.y * lightDistance1	+ lightAttenAndType1.z * lightDistance1 * lightDistance1 );
		float attenuation2 = 1.0f / ( lightAttenAndType2.x + lightAttenAndType2.y * lightDistance2	+ lightAttenAndType2.z * lightDistance2 * lightDistance2 );
		float attenuation3 = 1.0f / ( lightAttenAndType3.x + lightAttenAndType3.y * lightDistance3	+ lightAttenAndType3.z * lightDistance3 * lightDistance3 );
		
		/* vec3 lightReachingObject1 = lightColour1 * attenuation1;
		vec3 lightReachingObject2 = lightColour2 * attenuation2;
		vec3 lightReachingObject3 = lightColour3 * attenuation3;
		
		outDiffuse.rgb += (color * lightReachingObject1);
		outDiffuse.rgb += (color * lightReachingObject2);
		outDiffuse.rgb += (color * lightReachingObject3); */
		
		outDiffuse += (lightColour1 - color) * attenuation1;
		outDiffuse += (lightColour2 - color) * attenuation2;
		outDiffuse += (lightColour3 - color) * attenuation3;
		
	}else{
		outDiffuse = color;
	}

    gl_FragColor = vec4(outDiffuse, 1.0);
};






//	gl_FragColor.rgb *= 0.0001f;		// Almost zero	
//	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );

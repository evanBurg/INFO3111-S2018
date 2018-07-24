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
	vec3 outDiffuse = vec3(0.0f, 0.0f, 0.0f); 		// Final output colour
	
	vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);		// Light colour (in this case, bright white)
	
	//*****************************
	// Now we calculate the "contribution" of the light
	// onto this object. Here, I'm only using one light,
	// but you would calculate multiple lights for the 
	// checkpoint. 
	
	// New scope brace. (perhaps place inside a loop? Hint, hint...)
	{
		// Calcuate the amount of light hitting the object
		// (for THIS, specific, light)
		

		// Calculate the distance between the light 
		// and the vertex that this fragment is using
		float lightDistance = distance( vertInWorld, lightPosition );
		lightDistance = abs(lightDistance);

		float attenuation = 1.0f / 
			( lightAttenAndType.x 						// 0  
			+ lightAttenAndType.y * lightDistance					// Linear
			+ lightAttenAndType.z * lightDistance * lightDistance );	// Quad
		
		// Calculate how much light will hit the object at this distance:
		// (i.e. AFTER the light has been attenuated)
		
		vec3 lightReachingObject = lightColour * attenuation;

		// Now, add THIS lights light onto the object
		// Here we multiply the origin object colour with
		//  the light hitting the object from THIS light...
		//
		// The right side of the equation is the what colour
		//	this light adds to the object, which is a multiple
		//	of the light hitting the object multiplied by
		//	the colour of the object. 
		// 
		// Add this contribution to the overall colour
		// (i.e. from ALL lights, even though there's just one here)
		
		outDiffuse.rgb += ( color * lightReachingObject );
	}

	// Pass the final calculated colour out to the framebuffer (screen)
    gl_FragColor = vec4(outDiffuse, 1.0);
};





// Some old code we had before, but aren't using
//	gl_FragColor.rgb *= 0.0001f;		// Almost zero	
//	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );


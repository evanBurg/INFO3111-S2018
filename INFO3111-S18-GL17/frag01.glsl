#version 420
// FRAGMENT SHADER

// From the vertex shader:
in vec4 vertColourRGBA;
in vec4 vertWorldPosXYZ;
in vec4 vertNormal;
in vec4 vertTexUV;	

bool bDoTheSexyThing;

const int NUMLIGHTS = 1;

struct sLight
{
	vec3 Position;
	vec4 Direction;
	vec4 Diffuse;
	vec4 Ambient;
	vec4 Specular; 		// rgb = colour, w = intensity
	// x = constant, y = linear, z = quadratic, w = “type”
	vec4 AttenAndType;	// 0 = point, 1 = spot, 2 = directional 
};
uniform sLight theLights[NUMLIGHTS];


out vec4 outputColour;

void main()
{
	// Take the colour of the thing, and set the output 
	//	colour to that to start... 
	//vec3 outDiffuse = color; 
	vec4 outDiffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);	// Start with black 
	
	for ( int index = 0; index != NUMLIGHTS; index++ )
	{
		// Calculate the distance between the light 
		// and the vertex that this fragment is using
		float lightDistance = distance( vertWorldPosXYZ.xyz, theLights[index].Position );
		lightDistance = abs(lightDistance);

		float attenuation = 1.0f / 
			( theLights[index].AttenAndType.x 						// 0  
			+ theLights[index].AttenAndType.y * lightDistance					// Linear
			+ theLights[index].AttenAndType.z * lightDistance * lightDistance );	// Quad

// Optional clamp of attenuation, so the light won't get 'too' dark			
//		attenuation = clamp( attenuation, 0.0f, 1000.0f );

		vec3 lightContrib = theLights[index].Diffuse.rgb;
		
		lightContrib *= attenuation;
		
		outDiffuse.rgb += (vertColourRGBA.rgb * lightContrib);
	}
	
	outDiffuse.rgb = clamp( outDiffuse.rgb, vec3(0.0f,0.0f,0.0f), vec3(1.0f,1.0f,1.0f) );
	
	outputColour = vec4(outDiffuse.rgb, 1.0f);
};






//	gl_FragColor.rgb *= 0.0001f;		// Almost zero	
//	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );

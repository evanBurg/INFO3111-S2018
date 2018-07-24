#version 420
// FRAGMENT SHADER

// From the vertex shader:
in vec4 vertColourRGBA;
in vec4 vertWorldPosXYZ;
in vec4 vertNormal;
in vec4 vertTexUV;	

uniform bool bDontLightObject;

const int NUMLIGHTS = 3;

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

// LIGHT 1
uniform vec3 lightPosition1;
uniform vec4 lightDirection1;
uniform vec4 lightDiffuse1;
uniform vec4 lightAmbient1;
uniform vec4 lightSpecular1;
uniform vec4 lightAttenAndType1;		
uniform vec3 lightColour1;
//

out vec4 outputColour;

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
	
	if ( bDontLightObject )
	{
		outputColour = vertColourRGBA;
		// early exit
		return;	
	}

	// Take the colour of the thing, and set the output 
	//	colour to that to start... 

	// vec3 outDiffuse = vec3(0.0f, 0.0f, 0.0f); 
	// if(AffectedByLight){
		// // Calculate the distance between the light 
		// // and the vertex that this fragment is using
		// float lightDistance1 = abs(distance( vertInWorld, lightPosition1));
		// float lightDistance2 = abs(distance( vertInWorld, lightPosition2));
		// float lightDistance3 = abs(distance( vertInWorld, lightPosition3));

		// float attenuation1 = 1.0f / ( lightAttenAndType1.x + lightAttenAndType1.y * lightDistance1	+ lightAttenAndType1.z * lightDistance1 * lightDistance1 );
		// float attenuation2 = 1.0f / ( lightAttenAndType2.x + lightAttenAndType2.y * lightDistance2	+ lightAttenAndType2.z * lightDistance2 * lightDistance2 );
		// float attenuation3 = 1.0f / ( lightAttenAndType3.x + lightAttenAndType3.y * lightDistance3	+ lightAttenAndType3.z * lightDistance3 * lightDistance3 );
		
		// /* vec3 lightReachingObject1 = lightColour1 * attenuation1;
		// vec3 lightReachingObject2 = lightColour2 * attenuation2;
		// vec3 lightReachingObject3 = lightColour3 * attenuation3;
		
		// outDiffuse.rgb += (color * lightReachingObject1);
		// outDiffuse.rgb += (color * lightReachingObject2);
		// outDiffuse.rgb += (color * lightReachingObject3); */
		
		// outDiffuse += (lightColour1 - color) * attenuation1;
		// outDiffuse += (lightColour2 - color) * attenuation2;
		// outDiffuse += (lightColour3 - color) * attenuation3;
		
	// }else{
		// outDiffuse = color;
	// }

    // gl_FragColor = vec4(outDiffuse, 1.0);*
	
	//vec3 outDiffuse = color; 
	vec4 outDiffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);	// Start with black 
	
	for ( int index = 0; index != NUMLIGHTS; index++ )
	{
	
		// The Diffuse component (aka how much light is reflecting off the surface?)
		
		vec3 lightVector = theLights[index].Position - vertWorldPosXYZ.xyz;
		// 'normalize' means a vector of unit (1.0) length
		// (to make the math not screw up)
		lightVector = normalize(lightVector);
		
		// Get the dot product of the light and normalize
		float dotProduct = dot( lightVector, normalize(vertNormal.xyz) );	// -1 to 1
		
		dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
		vec3 lightContrib = theLights[index].Diffuse.rgb;
	
		lightContrib *= dotProduct;
	
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

		
		lightContrib *= attenuation;
		
		outDiffuse.rgb += (vertColourRGBA.rgb * lightContrib);
	}
	
	outDiffuse.rgb = clamp( outDiffuse.rgb, vec3(0.0f,0.0f,0.0f), vec3(1.0f,1.0f,1.0f) );
	
	outputColour = vec4(outDiffuse.rgb, 1.0f);
};






//	gl_FragColor.rgb *= 0.0001f;		// Almost zero	
//	gl_FragColor.rgb += vec3( 0.23f, 0.420f, 0.69f );

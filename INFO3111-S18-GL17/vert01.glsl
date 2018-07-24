#version 420
// VERTEX SHADER

in vec4 vPosition; 
in vec4 vColourRGBA;
in vec4 vNormal;
in vec4 vTexUV;


uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform vec4 meshColourRGBA; 		// Now a vec4!!
// If true, the A value in vColourRGBA
// will be used as the 'a' value for the colour
uniform bool bUse_vColourRGBA_AlphaValue;		

// glUniform1f()  pass in 0 or 1 
uniform bool bUseVertexColour;		// 0 or 1 Really a float

// Being passed to next shader stage... 
out vec4 vertColourRGBA;
out vec4 vertWorldPosXYZ;
out vec4 vertNormal;
out vec4 vertTexUV;


void main()
{
    vec4 newVertex = vPosition;		
	newVertex.w = 1.0f;
	
//    gl_Position = MVP * vec4(newVertex, 1.0);


	mat4 matMVP = matProjection * matView * matModel;
	// This is the "screen space" location
	gl_Position = matMVP * newVertex;
	
	// Calculate the "world" location of the  matrix.
	// (is vec4 because 4x4 times 4x1 is a 4x1)
	vertWorldPosXYZ = matModel * newVertex;

	// Calculate the "rotation only" transformation for the normals;
	vertNormal = inverse(transpose(matModel)) * normalize(vNormal);
	
	vertTexUV = vTexUV;
	
	vertColourRGBA.rgb = meshColourRGBA.rgb;
	// By default, we use the 'whole object' 
	// alpha (transparency) value...
	vertColourRGBA.a = meshColourRGBA.a;
	
	if ( !bUseVertexColour )
	{
		vertColourRGBA.rgb = vColourRGBA.rgb;	
	}
	// This will use the model file alpha value, instead
	// (Default is to NOT do this, though)
	if ( bUse_vColourRGBA_AlphaValue )
	{
		vertColourRGBA.a = vColourRGBA.a;
	}
};

#include "globalStuff.h"

#include "cVAOManager.h"

#include <fstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL


	return;
}


bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

	if ( ! this->m_LoadTheModel( fileName, drawInfo ) )
	{
		this->m_AppendTextToLastError( "Didn't load model", true );
		return false;
	}

	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVert) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  GL_STATIC_DRAW );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vPosition_location = glGetAttribLocation(shaderProgramID, "vPosition");	// program
	GLint vColourRGBA_location = glGetAttribLocation(shaderProgramID, "vColourRGBA");	// program;
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program
	GLint vTexUV_location = glGetAttribLocation(shaderProgramID, "vTexUV");	// program;

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vPosition_location);		// vPosition
	glVertexAttribPointer( vPosition_location, 4,		// vPosition
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVert),	// sizeof(float) * 6, 
						   ( void* ) offsetof(sVert, x) );

	glEnableVertexAttribArray(vColourRGBA_location);	// vColourRGBA
	glVertexAttribPointer( vColourRGBA_location, 4,		// vColourRGBA
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVert),	// sizeof(float) * 6, 
						   ( void* ) offsetof(sVert, red) );

	glEnableVertexAttribArray(vNormal_location);		// vNormal
	glVertexAttribPointer( vNormal_location, 4,		// vNormal
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVert),	// sizeof(sVert),	// sizeof(float) * 6, 
						   ( void* ) offsetof(sVert, nx) );

	glEnableVertexAttribArray(vTexUV_location);		// vTexUV
	glVertexAttribPointer( vTexUV_location, 4,		// vTexUV
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVert),	// sizeof(sVert),	// sizeof(float) * 6, 
						   ( void* ) offsetof(sVert, u1) );

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vPosition_location);
	glDisableVertexAttribArray(vColourRGBA_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vTexUV_location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}


bool cVAOManager::m_LoadTheModel(std::string fileName,
								 sModelDrawInfo &drawInfo )
{
	// Open the file. 
	// Read until we hit the word "vertex"
	// Read until we hit the word "face"
	// Read until we hit the word "end_header"

	std::ifstream thePlyFile( fileName.c_str() );
	if ( ! thePlyFile.is_open() )
	{	// Something is wrong...
		std::stringstream ssError;
		ssError << "Can't open >" << fileName << "< file." << std::endl;
		this->m_AppendTextToLastError(ssError.str());
		return false;
	}

	std::string temp; 
	while ( thePlyFile >> temp )
	{
		if ( temp == "vertex" ) 
		{
			break;
		}
	}; 
	// read the number of vertices...
	thePlyFile >> drawInfo.numberOfVertices;	//::g_NumberOfVertices;

	while ( thePlyFile >> temp )
	{
		if ( temp == "face" ) 
		{
			break;
		}
	}; 
	// read the number of triangles...
	thePlyFile >> drawInfo.numberOfTriangles;		// ::g_NumberOfTriangles;


	while ( thePlyFile >> temp )
	{
		if ( temp == "end_header" ) 
		{
			break;
		}
	}; 

	// And now, we start reading vertices... Hazzah!
	
	// This is set up to match the ply (3d model) file. 
	// NOT the shader. 
	struct sVertPly
	{
		glm::vec3 pos;
		glm::vec4 colour;
		// Added
		glm::vec3 normal;
		glm::vec2 texUV;
	};

	std::vector<sVertPly> vecTempPlyVerts;

	sVertPly tempVert;
	// Load the vertices...
	for ( unsigned int index = 0; index != drawInfo.numberOfVertices; // ::g_NumberOfVertices; 
		  index++ )
	{
		thePlyFile >> tempVert.pos.x >> tempVert.pos.y >> tempVert.pos.z;
		
		thePlyFile >> tempVert.normal.x >> tempVert.normal.y >> tempVert.normal.z;

		thePlyFile >> tempVert.colour.x >> tempVert.colour.y
			       >> tempVert.colour.z >> tempVert.colour.w; 

		thePlyFile >> tempVert.texUV.x >> tempVert.texUV.y;

		// Scale the colour from 0 to 1, instead of 0 to 255
		tempVert.colour.x /= 255.0f;
		tempVert.colour.y /= 255.0f;
		tempVert.colour.z /= 255.0f;
		tempVert.colour.a /= 255.0f;

		// Add too... what? 
		vecTempPlyVerts.push_back(tempVert);
	}

	// Calculate the extents of the model
	// Assume the 1st one is both the min and max
	drawInfo.minX = drawInfo.maxX = vecTempPlyVerts[0].pos.x;
	drawInfo.minY = drawInfo.maxY = vecTempPlyVerts[0].pos.y;
	drawInfo.minZ = drawInfo.maxZ = vecTempPlyVerts[0].pos.z;

	for ( std::vector<sVertPly>::iterator itVert = vecTempPlyVerts.begin(); 
		  itVert != vecTempPlyVerts.end(); itVert++ )
	{
		if ( itVert->pos.x > drawInfo.maxX )	{ drawInfo.maxX = itVert->pos.x; }
		if ( itVert->pos.y > drawInfo.maxY )	{ drawInfo.maxY = itVert->pos.y; }
		if ( itVert->pos.z > drawInfo.maxZ )	{ drawInfo.maxZ = itVert->pos.z; }

		if ( itVert->pos.x < drawInfo.minX )	{ drawInfo.minX = itVert->pos.x; }
		if ( itVert->pos.y < drawInfo.minY )	{ drawInfo.minY = itVert->pos.y; }
		if ( itVert->pos.z < drawInfo.minZ )	{ drawInfo.minZ = itVert->pos.z; }
	}//for ( std::vector<sVertPly>::iterator itVert...

	drawInfo.extentX = drawInfo.maxX - drawInfo.minX;
	drawInfo.extentY = drawInfo.maxY - drawInfo.minY;
	drawInfo.extentZ = drawInfo.maxZ - drawInfo.minZ;

	// calculate the maximum extent
	drawInfo.maxExtent = drawInfo.extentX;
	if ( drawInfo.extentY > drawInfo.maxExtent )	{ drawInfo.maxExtent = drawInfo.extentY; }
	if ( drawInfo.extentZ > drawInfo.maxExtent )	{ drawInfo.maxExtent = drawInfo.extentZ; }


	// Create a local vertex array
	// Note: The format the file (ply) is DIFFERENT from this array:
	// - sVertPly was made to match the file format
	// - sVert was made to match the shader vertex attrib format

	drawInfo.pVertices = new sVert[drawInfo.numberOfVertices];

	// Optional clear array to zero 
	//memset( drawInfo.pVertices, 0, sizeof(sVert) * drawInfo.numberOfVertices);

	for ( unsigned int index = 0; index != drawInfo.numberOfVertices; index++ )
	{
		drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
		drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
		drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;

		drawInfo.pVertices[index].red = vecTempPlyVerts[index].colour.r;
		drawInfo.pVertices[index].green = vecTempPlyVerts[index].colour.g;
		drawInfo.pVertices[index].blue = vecTempPlyVerts[index].colour.b;
		drawInfo.pVertices[index].alpha = vecTempPlyVerts[index].colour.a;

		drawInfo.pVertices[index].nx = vecTempPlyVerts[index].normal.x;
		drawInfo.pVertices[index].ny = vecTempPlyVerts[index].normal.y;
		drawInfo.pVertices[index].nz = vecTempPlyVerts[index].normal.z;
		drawInfo.pVertices[index].n_discard = 1.0f;

		drawInfo.pVertices[index].u1 = vecTempPlyVerts[index].texUV.x;
		drawInfo.pVertices[index].v1 = vecTempPlyVerts[index].texUV.y;
		drawInfo.pVertices[index].u2 = vecTempPlyVerts[index].texUV.x;
		drawInfo.pVertices[index].v2 = vecTempPlyVerts[index].texUV.y;

	}// for ( unsigned int index...


	struct sTriPly
	{
		unsigned int vindex[3];		// the 3 indices
		sVertPly verts[3];			// The actual vertices
	};

	std::vector<sTriPly> vecTempPlyTriangles;
	sTriPly tempTriangle;
	unsigned int discard = 0;
	for ( unsigned int index = 0; index != drawInfo.numberOfTriangles;	// ::g_NumberOfTriangles; 
		  index++ )
	{
		// 3 5622 5601 5640
		thePlyFile >> discard						// the "3" at the start
			       >> tempTriangle.vindex[0]
			       >> tempTriangle.vindex[1]
			       >> tempTriangle.vindex[2];

		//// Look up the vertex that matches the triangle index values.
		//tempTriangle.verts[0] = vecTempPlyVerts[ tempTriangle.vindex[0] ];
		//tempTriangle.verts[1] = vecTempPlyVerts[ tempTriangle.vindex[1] ];
		//tempTriangle.verts[2] = vecTempPlyVerts[ tempTriangle.vindex[2] ];

		vecTempPlyTriangles.push_back( tempTriangle );
	}//for ( unsigned int index...

	
	// NOW, we need to put them into the vertex array buffer that 
	//	will be passed to OpenGL. Why? 
	// Because we called glDrawArrays(), that's why. 

//	::g_NumberOfVertsToDraw = ::g_NumberOfTriangles * 3;	// 3 because "triangles"

	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

	// sVert* pVertices = 0;
//	pVertices = new sVert[::g_NumberOfVertsToDraw];

	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	// Optional clear array to zero 
	//memset( drawInfo.pIndices, 0, sizeof(unsigned int) * drawInfo.numberOfIndices);

	// Allocate on the HEAP, so infinite size... 
	// delete pVertices			/// error!
	// delete [] pVertices		/// correct!!
	unsigned int indexBufferIndex = 0;
	for ( unsigned int triIndex = 0; 
		  triIndex != drawInfo.numberOfTriangles;		// ::g_NumberOfTriangles; 
		  triIndex++, indexBufferIndex += 3 )
	{
		sTriPly& curTri = vecTempPlyTriangles[triIndex];

		// This is taking an array of triangles vertex indices 
		// (which is a 2D array of indices, really) and converting
		//  it into a 1D array of indices...)
		drawInfo.pIndices[indexBufferIndex + 0] = curTri.vindex[0];
		drawInfo.pIndices[indexBufferIndex + 1] = curTri.vindex[1];
		drawInfo.pIndices[indexBufferIndex + 2] = curTri.vindex[2];

	}// for ( unsigned int triIndex = 0...

	return true;
}


std::string cVAOManager::getLastError(bool bAndClear /*=true*/)
{
	std::string theLastError = this->m_lastErrorString;

	if ( bAndClear )
	{
		this->m_lastErrorString = "";
	}

	return theLastError;
}

void cVAOManager::m_AppendTextToLastError(std::string text, bool addNewLineBefore /*=true*/)
{
	std::stringstream ssError;
	ssError << this->m_lastErrorString;

	if ( addNewLineBefore )
	{
		ssError << std::endl;
	}

	ssError << text;

	this->m_lastErrorString = ssError.str();

	return;

}

void cVAOManager::ShutDown(void)
{

	for ( std::map< std::string, sModelDrawInfo >::iterator itVAO = this->m_map_ModelName_to_VAOID.begin();
		  itVAO != this->m_map_ModelName_to_VAOID.end(); itVAO++ )
	{
		delete [] itVAO->second.pVertices;
		delete [] itVAO->second.pIndices;

		glGenVertexArrays;
		glDeleteVertexArrays( 1, &(itVAO->second.VAO_ID) );
		glDeleteBuffers( 1, &(itVAO->second.VertexBufferID) );
		glDeleteBuffers( 1, &(itVAO->second.IndexBufferID) );
	}

	return;
}

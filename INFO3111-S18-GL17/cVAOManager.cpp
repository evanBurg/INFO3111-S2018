#include "cVAOManager.h"

#include "globalOpenGLStuff.h"

#include <fstream>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo)

{

	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);

	// sVert vertices[3]
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVert) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  drawInfo.pVertices,							// pVertices,			//vertices, 
				  GL_STATIC_DRAW);


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{



	return true;
}


void cVAOManager::m_LoadTheModel(std::string fileName,
								 sModelDrawInfo &drawInfo )
{
	// Open the file. 
	// Read until we hit the word "vertex"
	// Read until we hit the word "face"
	// Read until we hit the word "end_header"

	std::ifstream thePlyFile( fileName.c_str() );
	if ( ! thePlyFile.is_open() )
	{	// Something is wrong...
		return;
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
	};

	std::vector<sVertPly> vecTempPlyVerts;

	sVertPly tempVert;
	// Load the vertices...
	for ( unsigned int index = 0; index != drawInfo.numberOfVertices; // ::g_NumberOfVertices; 
		  index++ )
	{
		thePlyFile >> tempVert.pos.x >> tempVert.pos.y >> tempVert.pos.z;
		
//		tempVert.pos.x *= 10.0f;
//		tempVert.pos.y *= 10.0f;
//		tempVert.pos.z *= 10.0f;


		thePlyFile >> tempVert.colour.x >> tempVert.colour.y
			       >> tempVert.colour.z >> tempVert.colour.w; 

		// Scale the colour from 0 to 1, instead of 0 to 255
		tempVert.colour.x /= 255.0f;
		tempVert.colour.y /= 255.0f;
		tempVert.colour.z /= 255.0f;

		// Add too... what? 
		vecTempPlyVerts.push_back(tempVert);
	}

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

		// Look up the vertex that matches the triangle index values.
		tempTriangle.verts[0] = vecTempPlyVerts[ tempTriangle.vindex[0] ];
		tempTriangle.verts[1] = vecTempPlyVerts[ tempTriangle.vindex[1] ];
		tempTriangle.verts[2] = vecTempPlyVerts[ tempTriangle.vindex[2] ];

		vecTempPlyTriangles.push_back( tempTriangle );
	}
	
	// NOW, we need to put them into the vertex array buffer that 
	//	will be passed to OpenGL. Why? 
	// Because we called glDrawArrays(), that's why. 

	::g_NumberOfVertsToDraw = ::g_NumberOfTriangles * 3;	// 3 because "triangles"

	// sVert* pVertices = 0;
//	pVertices = new sVert[::g_NumberOfVertsToDraw];
	drawInfo.pVertices = new sVert[drawInfo.numb];

	// Allocate on the HEAP, so infinite size... 
	// delete pVertices			/// error!
	// delete [] pVertices		/// correct!!
	unsigned int vertIndex = 0;
	for ( unsigned int triIndex = 0; 
		  triIndex != ::g_NumberOfTriangles; 
		  triIndex++, vertIndex += 3 )
	{
		sTriPly& curTri = vecTempPlyTriangles[triIndex];

		pVertices[ vertIndex + 0 ].x = curTri.verts[0].pos.x;
		pVertices[ vertIndex + 0 ].y = curTri.verts[0].pos.y;
		pVertices[ vertIndex + 0 ].z = curTri.verts[0].pos.z;
		pVertices[ vertIndex + 0 ].r = curTri.verts[0].colour.x;
		pVertices[ vertIndex + 0 ].g = curTri.verts[0].colour.y;
		pVertices[ vertIndex + 0 ].b = curTri.verts[0].colour.z;

		pVertices[ vertIndex + 1 ].x = curTri.verts[1].pos.x;
		pVertices[ vertIndex + 1 ].y = curTri.verts[1].pos.y;
		pVertices[ vertIndex + 1 ].z = curTri.verts[1].pos.z;
		pVertices[ vertIndex + 1 ].r = curTri.verts[1].colour.x;
		pVertices[ vertIndex + 1 ].g = curTri.verts[1].colour.y;
		pVertices[ vertIndex + 1 ].b = curTri.verts[1].colour.z;

		pVertices[ vertIndex + 2 ].x = curTri.verts[2].pos.x;
		pVertices[ vertIndex + 2 ].y = curTri.verts[2].pos.y;
		pVertices[ vertIndex + 2 ].z = curTri.verts[2].pos.z;
		pVertices[ vertIndex + 2 ].r = curTri.verts[2].colour.x;
		pVertices[ vertIndex + 2 ].g = curTri.verts[2].colour.y;
		pVertices[ vertIndex + 2 ].b = curTri.verts[2].colour.z;

	}// for ( unsigned int triIndex = 0...

	return;
}
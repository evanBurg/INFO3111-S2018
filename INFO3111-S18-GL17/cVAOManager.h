#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>

// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVert
{
	float x, y, z, w;			// vPosition
	float red, green, blue, alpha;		// vColourRGBA
	float nx, ny, nz, n_discard;		// vNormal
	float u1, v1, u2, v2;	// vTexUV
	sVert() : 
		x(0.0f), y(0.0f), z(0.0f), w(0.0f), 
		red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f),
		nx(0.0f), ny(0.0f), nz(0.0f), n_discard(0.0f),
		u1(0.f), v1(0.0f), u2(0.0f), v2(0.0f) 
	{};
};


struct sModelDrawInfo
{
	sModelDrawInfo(); 

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVert* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	float maxX, maxY, maxZ;
	float minX, minY, minZ;
	float extentX, extentY, extentZ;
	float maxExtent;

};


class cVAOManager
{
public:

	bool LoadModelIntoVAO(std::string fileName, 
						  sModelDrawInfo &drawInfo, 
						  unsigned int shaderProgramID);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename,
								 sModelDrawInfo &drawInfo);

	std::string getLastError(bool bAndClear = true);

	void ShutDown(void);

private:

	std::map< std::string /*model name*/,
		      sModelDrawInfo /* info needed to draw*/ >
		m_map_ModelName_to_VAOID;

	// Loads the ply model file into a temporary array
	bool m_LoadTheModel( std::string fileName, 
						 sModelDrawInfo &drawInfo);

	std::string m_lastErrorString;
	void m_AppendTextToLastError(std::string text, bool addNewLineBefore = true);
};

#endif	// _cVAOManager_HG_

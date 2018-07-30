#ifndef _cMeshObject_HG_
#define _cMeshObject_HG_

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4

class cMeshObject
{
public:
	cMeshObject();
	cMeshObject(std::string meshName, glm::vec3 pos, glm::vec4 colour, glm::vec3 orientation, float scale, bool wireframe);
    ~cMeshObject();
    std::string meshName;
    glm::vec3 pos;			// 0,0,0 (origin)
    glm::vec4 colour;		// 0,0,0,1 (black)
	glm::vec3 orientation;	// 
    float scale;			// 1.0f
    bool isWireframe;		// false
	bool bDontLightObject;

	const unsigned int uniqueID = 0;

	bool bIsVisible; 

	// This is the name of the object that makes sense to us humans
	std::string friendlyName;

	// Basic physics integration
	// (NOT on mid-term, but handy)
	glm::vec3 velocity;
	glm::vec3 acceleration;

private:
	void m_AssignUniqueID(void);
	static unsigned int m_nextID;	// = 1000
};


#endif // _cMeshObject_HG_

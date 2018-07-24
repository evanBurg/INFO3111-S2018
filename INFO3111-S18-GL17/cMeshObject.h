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
	bool isUniformColour;
	bool isAffectedByLight;
};

#endif // _cMeshObject_HG_

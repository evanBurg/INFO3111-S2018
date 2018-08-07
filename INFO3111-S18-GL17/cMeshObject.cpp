#include "cMeshObject.h"

cMeshObject::cMeshObject()
{
	this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
    this->colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    this->scale = 1.0f;
    this->isWireframe = false;
	this->orientation = glm::vec3(0.0f,0.0f,0.0f);

	this->m_AssignUniqueID();

	this->bDontLightObject = false;

	this->bIsVisible = true;

	this->colourSource = cMeshObject::USE_VERTEX_COLOURS;

	this->bUseColourAlphaValue = true;


	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);


	return;
}

cMeshObject::cMeshObject(std::string meshName, glm::vec3 pos, glm::vec4 colour, glm::vec3 orientation, float scale, bool wireframe) {
	this->meshName = meshName;
	this->pos = pos;
	this->colour = colour;
	this->orientation = orientation;
	this->scale = scale;
	this->isWireframe = wireframe;
}

cMeshObject::~cMeshObject()
{

	return;
}

//static 
unsigned int cMeshObject::m_nextID = 1000;

	//mutable unsigned int uniqueID;
	//const unsigned int uniqueID = 0;

void cMeshObject::m_AssignUniqueID(void) 
{
	unsigned int* pUniqeID = const_cast<unsigned int*>(&this->uniqueID);

	(*pUniqeID) = cMeshObject::m_nextID;

	cMeshObject::m_nextID++;
	return;
}

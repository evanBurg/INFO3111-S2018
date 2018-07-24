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

	return;
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

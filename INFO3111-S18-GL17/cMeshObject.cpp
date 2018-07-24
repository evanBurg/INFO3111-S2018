#include "cMeshObject.h"

cMeshObject::cMeshObject()
{
	this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
    this->colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    this->scale = 1.0f;
    this->isWireframe = false;
	this->orientation = glm::vec3(0.0f,0.0f,0.0f);
	this->isUniformColour = false;
	this->isAffectedByLight = true;

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

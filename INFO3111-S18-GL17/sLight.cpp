#include "sLight.h"

//static 
const float sLight::MAX_ATTENUATION = 100.0f;

sLight::sLight()
{
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->diffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->attenConst = 0.0f;
	this->attenLinear = 0.1f;
	this->attenQuad = 0.1f;
	this->UniLoc_Position = -1;
	this->UniLoc_Diffuse = -1;
	this->UniLoc_AttenAndLight = -1;

	this->TurnLightOff();

	return;
}

// These set the w (4th) value of the diffuse component 
void sLight::TurnLightOff(void)
{
	this->diffuseColour.w = 0.0f;
	return;
}

void sLight::TurnLightOn(void)
{
	this->diffuseColour.w = 1.0f;
	return;
}

bool sLight::IsOn(void)
{
	if ( this->diffuseColour.w != 0.0f )
	{
		return true;
	}
	return false;
}

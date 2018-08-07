#ifndef _sLight_HG_
#define _sLight_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// This will get more involved
struct sLight
{
	sLight();

	glm::vec3 position;
	glm::vec4 diffuseColour;
	float attenConst;
	float attenLinear;
	float attenQuad;

	// Was GLuint, but changed to avoid including globalOpenGLStuff.h
	unsigned int UniLoc_Position;
	unsigned int UniLoc_Diffuse;
	unsigned int UniLoc_AttenAndLight;

	static const float MAX_ATTENUATION;// = 100.0f;

	// These set the w (4th) value of the diffuse component 
	bool IsOn(void);
	void TurnLightOff(void);
	void TurnLightOn(void);
};



#endif

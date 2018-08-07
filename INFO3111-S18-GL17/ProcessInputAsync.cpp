#include "globalStuff.h"

#include <sstream>

void ProcessInputAsync( glm::vec3 &cameraEye, glm::vec3 &cameraTarget, GLFWwindow* &window )
{
	float cameraSpeed = 0.05f; 

	int state = glfwGetKey(window, GLFW_KEY_D);
	if (state == GLFW_PRESS) { cameraEye.x += cameraSpeed; }

	state = glfwGetKey(window, GLFW_KEY_A);
	if (state == GLFW_PRESS) { cameraEye.x -= cameraSpeed; }

	state = glfwGetKey(window, GLFW_KEY_W);
	if (state == GLFW_PRESS) { cameraEye.z += cameraSpeed; }

	state = glfwGetKey(window, GLFW_KEY_S);
	if (state == GLFW_PRESS) { cameraEye.z -= cameraSpeed; }

	state = glfwGetKey(window, GLFW_KEY_Q);	// Up
	if (state == GLFW_PRESS) { cameraEye.y += cameraSpeed; }

	state = glfwGetKey(window, GLFW_KEY_E);	// Down
	if (state == GLFW_PRESS) { cameraEye.y -= cameraSpeed; }

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->pos.z += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->pos.z -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->pos.x -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->pos.x += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->pos.y += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->pos.y -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->orientation.z += cameraSpeed / 4;
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->orientation.z -= cameraSpeed / 4;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->orientation.x -= cameraSpeed / 4;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->orientation.x += cameraSpeed / 4;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->orientation.y += cameraSpeed / 4;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		::g_vec_pMeshObjects[::g_SelectedObjectID]->orientation.y -= cameraSpeed / 4;
	}

	if ( glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS )
	{	// Decrease linear atten by 1%
		::g_vecLights[0].attenLinear *= 0.99f;
	}

	if ( glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS )
	{	// Increase linear atten by 1%
		if ( ::g_vecLights[::g_SelectedLightID].attenLinear <= 0.0f )	 
		{ 
			::g_vecLights[::g_SelectedLightID].attenLinear = 0.001f;		// Make it a tiny value
		}
		else
		{
			::g_vecLights[::g_SelectedLightID].attenLinear *= 1.01f;		// + 1%
			if ( ::g_vecLights[::g_SelectedLightID].attenLinear >= sLight::MAX_ATTENUATION )
			{
				::g_vecLights[::g_SelectedLightID].attenLinear = sLight::MAX_ATTENUATION;		
			}
		}
	}
	if ( glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS )
	{	// Decrease quadratic atten by 1%
		::g_vecLights[::g_SelectedLightID].attenQuad *= 0.99f;			// +1%
	}

	if ( glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS )
	{	// Increase quadratic atten by 1%
		if ( ::g_vecLights[::g_SelectedLightID].attenQuad <= 0.0f )	 
		{ 
			::g_vecLights[::g_SelectedLightID].attenQuad = 0.0001f;		// Make it a tiny value
		}
		else
		{
			::g_vecLights[::g_SelectedLightID].attenQuad *= 1.01f;		// + 1%
			if ( ::g_vecLights[::g_SelectedLightID].attenQuad >= sLight::MAX_ATTENUATION )
			{
				::g_vecLights[::g_SelectedLightID].attenQuad = sLight::MAX_ATTENUATION;		
			}
		}
	}	

	// Also move the light around
	if ( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS )
	{
		::g_vecLights[::g_SelectedLightID].position.z += cameraSpeed;		
	}
	if ( glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS )
	{
		::g_vecLights[::g_SelectedLightID].position.z -= cameraSpeed;		
	}		
	if ( glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS )
	{
		::g_vecLights[::g_SelectedLightID].position.x -= cameraSpeed;		
	}	
	if ( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS )
	{
		::g_vecLights[::g_SelectedLightID].position.x += cameraSpeed;		
	}		
	if ( glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS )
	{
		::g_vecLights[::g_SelectedLightID].position.y += cameraSpeed;		
	}		
	if ( glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS )
	{
		::g_vecLights[::g_SelectedLightID].position.y -= cameraSpeed;		
	}		

	std::stringstream ssTitle;
	ssTitle << (::g_lookAtObject ? "**" : "") << "Selected Object: " << ::g_vec_pMeshObjects[::g_SelectedObjectID]->meshName << " | " << (!g_lookAtObject ? "**" : "") << "Light[" << ::g_SelectedLightID << "]: xyz("
		<< ::g_vecLights[::g_SelectedLightID].position.x << ", "
		<< ::g_vecLights[::g_SelectedLightID].position.y << ", "
		<< ::g_vecLights[::g_SelectedLightID].position.z << ") "
		<< "Lin: "
		<< ::g_vecLights[::g_SelectedLightID].attenLinear << " "
		<< "Quad: "
		<< ::g_vecLights[::g_SelectedLightID].attenQuad;
	glfwSetWindowTitle(::g_window, ssTitle.str().c_str());

	return;
}
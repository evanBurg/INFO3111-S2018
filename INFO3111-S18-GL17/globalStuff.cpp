#include "globalStuff.h"

std::vector< cMeshObject* > g_vec_pMeshObjects;

//cMeshObject* g_pTheLightMesh = 0;		// or NULL
// Will show the attenuation of the lights
//  1% brightness
// 25% brightness
// 50% brightness
// 90% brightness
//cMeshObject* g_pTheLightAttenMesh[4] = {0};		// or NULL
cMeshObject* g_pDebugSphere = NULL; 

// If true, the spheres around the lights are there
bool g_bTurnOnDebugLightSpheres = true;
bool g_lookAtObject = true;
// Use can do this to "select" different lights
unsigned int g_SelectedLightID = 0;
// Use can do this to "select" different objects
unsigned int g_SelectedObjectID = 0;


cMeshObject* g_pFindObjectByFriendlyName(std::string name)
{
	unsigned int numObject = (unsigned int) ::g_vec_pMeshObjects.size();
	for ( unsigned int index = 0; index != numObject; index++ )
	{
		// Is this zee one? 
		if ( ::g_vec_pMeshObjects[index]->friendlyName == name )
		{ 
			// Yup
			return ::g_vec_pMeshObjects[index];
		}
	}//for ( unsigned int index = 0

	return NULL;
}

//const unsigned int NUMLIGHTS = 10;
std::vector<sLight> g_vecLights;

// When true, the DoPhysicsUpdate is called.
bool g_bDoEulerPhysicsUpdate = false;	


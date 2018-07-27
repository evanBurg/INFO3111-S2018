#ifndef _commonEngineStuff_HG_
#define _commonEngineStuff_HG_

#include <vector>
#include "cMeshObject.h"

// Put common stuff in the header 
// ALWAYS use header variables as extern
// NEVER "regular" variables

extern std::vector< cMeshObject* > g_vec_pMeshObjects;

extern cMeshObject* g_pTheLightMesh;		// or NULL
extern cMeshObject* g_pTheLightAttenMesh[4];		// or NULL

// And common functions that are called.
// (signatures here)
void LoadObjectsIntoScene(void);

// If true, the spheres around the lights are there
extern bool g_bTurnOnDebugLightSpheres;	// = false
// Use can do this to "select" different lights
extern unsigned int g_SelectedLightID;	// = 0

// Returns NULL if not found
cMeshObject* g_pFindObjectByFriendlyName(std::string name);
cMeshObject* g_pFindObjectByUniqueID(unsigned int uniqueID);


void DoPhysicsIntegrationUpdate(double deltaTime);


#endif

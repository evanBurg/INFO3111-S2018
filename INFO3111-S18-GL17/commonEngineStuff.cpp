#include "commonEngineStuff.h"

std::vector< cMeshObject* > g_vec_pMeshObjects;

cMeshObject* g_pTheLightMesh = 0;		// or NULL

// Will show the attenuation of the lights
//  1% brightness
// 25% brightness
// 50% brightness
// 90% brightness
cMeshObject* g_pTheLightAttenMesh[4] = {0};		// or NULL
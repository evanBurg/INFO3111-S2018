#include "globalStuff.h"

#include <glm/glm.hpp>
#include <sstream>

//extern std::vector< cMeshObject* > g_vec_pMeshObjects;
//
//extern cMeshObject* g_pTheLightMesh = 0;		// or NULL

// To handle the scaling of the object
#include "cVAOManager.h"	
// Should be in a header somewhere... 
extern cVAOManager* g_pTheVAOManager;// = 0;



//std::vector< cMeshObject* > g_vec_pMeshObjects;
void LoadObjectsIntoScene(void)
{
	// The "debug sphere" that replaces all of the other spheres for the lights, etc.
	{// Add an object into the "scene"
		::g_pDebugSphere = new cMeshObject(); 

		::g_pDebugSphere->meshName = "isosphere_smooth_xyz_n_rgba_uv.ply";

		::g_pDebugSphere->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_pDebugSphere->colour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );

		// Get the "unit" scale from the VAOManager
		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( ::g_pDebugSphere->meshName, 
														modelInfo );
		//::g_pTheLightMesh->scale = 0.1f;
		::g_pDebugSphere->scale = 1.0f / modelInfo.maxExtent;

		::g_pDebugSphere->isWireframe = false;

		::g_pDebugSphere->bDontLightObject = true;

		// DON'T put this into the "scene" object (the "vector of things to draw")
		//::g_vec_pMeshObjects.push_back( ::g_pTheLightMesh );
	}	

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "CrappyTerrain_xyz_n_rgba_uv.ply";

		pTemp->pos = glm::vec3( 0.0f, -15.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 
								   205.0f/255.0f,
									49.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 1.0f;
		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}	


	return;
}

bool LoadModelTypes(GLint shadProgID, std::vector<std::string> vecModelNames, std::string &errors)
{
	bool bAllGood = true;

	std::stringstream ssError;

	for ( std::vector<std::string>::iterator itModelName = vecModelNames.begin(); 
		  itModelName != vecModelNames.end(); itModelName++ )
	{
		sModelDrawInfo theModel;
		if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( *itModelName, theModel, shadProgID ) )
		{
			ssError << "ERROR: " << *itModelName << " wasn't loaded" << std::endl;
			bAllGood = false;
		}
	}// for ( ...iterator itModelName...

	errors = ssError.str();

	return bAllGood;
}



bool LoadModelTypes(GLint shadProgID, std::string &errors)
{
	std::stringstream ssError;

	bool bAllGood = true;

	sModelDrawInfo terrain;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "CrappyTerrain_xyz_n_rgba_uv.ply", terrain, shadProgID ) )
	{
		ssError << "ERROR: CrappyTerrain_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
		bAllGood = false;
	}

	sModelDrawInfo sphere;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "isosphere_smooth_xyz_n_rgba_uv.ply", sphere, shadProgID ) )
	{
		ssError << "ERROR: isosphere_smooth_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
		bAllGood = false;
	}

	errors = ssError.str();

	return bAllGood;
}

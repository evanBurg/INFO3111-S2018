//#include <vector>
//#include "cMeshObject.h"
#include <glm/glm.hpp>

#include "commonEngineStuff.h"
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


	{// Add an object into the "scene"
		::g_pTheLightMesh = new cMeshObject(); 

		::g_pTheLightMesh->meshName = "isosphere_smooth_xyz_n_rgba_uv.ply";

		::g_pTheLightMesh->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_pTheLightMesh->colour = glm::vec4( 142.0f/255.0f, 
								   205.0f/255.0f,
									49.0f/255.0f,
									 1.0f );		// Transparency 'alpha'

		// Get the "unit" scale from the VAOManager
		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( ::g_pTheLightMesh->meshName, 
													  modelInfo );
		//::g_pTheLightMesh->scale = 0.1f;
		::g_pTheLightMesh->scale = 1.0f / modelInfo.maxExtent;


		::g_pTheLightMesh->isWireframe = false;

		::g_pTheLightMesh->bDontLightObject = true;

		::g_vec_pMeshObjects.push_back( ::g_pTheLightMesh );
	}	
	// Load the spheres to show the attenuation
	for ( unsigned int index = 0; index != 4; index++ )
	{
		::g_pTheLightAttenMesh[index] = new cMeshObject(); 

		::g_pTheLightAttenMesh[index]->meshName = "isosphere_smooth_xyz_n_rgba_uv.ply";

		::g_pTheLightAttenMesh[index]->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_pTheLightAttenMesh[index]->colour = glm::vec4( 142.0f/255.0f, 
								   205.0f/255.0f,
									49.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		::g_pTheLightAttenMesh[index]->scale = 0.1f;
		::g_pTheLightAttenMesh[index]->isWireframe = true;

		::g_pTheLightAttenMesh[index]->bDontLightObject = true;

		::g_vec_pMeshObjects.push_back( ::g_pTheLightAttenMesh[index] );
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
	
	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "cow_xyz_n_rgba_uv.ply";

		pTemp->pos = glm::vec3( 1.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 243.0f/255.0f,		
								     9.0f/255.0f,
								    25.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( pTemp->meshName, 
													  modelInfo );
		//pTemp->scale = 0.05f;
		pTemp->scale = 1.0f / modelInfo.maxExtent;

		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "cow_xyz_n_rgba_uv.ply";

		pTemp->pos = glm::vec3( 2.0f, 1.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f,	
								   205.0f/255.0f,
								   248.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( pTemp->meshName, 
													  modelInfo );
		//pTemp->scale = 0.1f;
		pTemp->scale = 1.0f / modelInfo.maxExtent;

		pTemp->isWireframe = true;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "bun_zipper_res2_xyz_n_rgba_uv.ply";

		pTemp->friendlyName = "Bugs";		// as in Bugs Bunny

		pTemp->velocity.y = 2.0f;

		pTemp->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 1.0f,
								   1.0f, 
									1.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( pTemp->meshName, 
													  modelInfo );
		//pTemp->scale = 2.0f;
		pTemp->scale = 1.0f / modelInfo.maxExtent;

		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "X-Wing_Attack_(33569 faces)_xyz_n_rgba_uv.ply";

		pTemp->friendlyName = "Luke";		

		pTemp->pos = glm::vec3( 0.0f, 2.0f, 0.0f );
		pTemp->colour = glm::vec4( 1.0f,  1.0f, 1.0f, 1.0f );		// Transparency 'alpha'
		pTemp->orientation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f );

		// 1.0 unit per second
//		pTemp->velocity.z = 0.1f;
		pTemp->acceleration.z = 0.01f;


		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( pTemp->meshName, 
													  modelInfo );
		//pTemp->scale = 2.0f;
		pTemp->scale = 1.0f / modelInfo.maxExtent;
		pTemp->scale *= 2.0f;			// a bit bigger

		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}
	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "ssj100_xyz_n_rgba_uv.ply";

		// 2 * PI   
		// 1 PI = 180
		// 0.5 = 90 
		// 0.25 = 45


//		pTemp->orientation.x = 3.14159f * 0.25f;		// YOLO, right? 
		pTemp->orientation.y = 3.14159f * 0.5f;		// YOLO, right? 

		pTemp->pos = glm::vec3( -1.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 
									49.0f/255.0f,
								   205.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( pTemp->meshName, 
													  modelInfo );
		//pTemp->scale = 1.5f;
		pTemp->scale = 1.0f / modelInfo.maxExtent;

		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "free_arena_ASCII_xyz_n_rgba_uv.ply";

		pTemp->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 244.0f/255.0f,  
									223.0f/255.0f,
								    33.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		// Largest "extent" in this model
		// is 40.2828 
		
		sModelDrawInfo modelInfo;
		::g_pTheVAOManager->FindDrawInfoByModelName ( pTemp->meshName, 
													  modelInfo );
		//pTemp->scale = 1.0f/40.2828f;
		pTemp->scale = 1.0f / modelInfo.maxExtent;


		// Now my model 1.0 unit in size (-1 to 1)

		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}


	return;
}
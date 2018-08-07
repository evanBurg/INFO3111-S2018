#ifndef _globalStuff_HG_
#define _globalStuff_HG_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include <vector>
#include "cMeshObject.h"
#include "sLight.h"

#include "CGLColourHelper.h"

#include "cShaderManager.h"

#include "cVAOManager.h"

// Put common stuff in the header 
// ALWAYS use header variables as extern
// NEVER "regular" variables


extern GLFWwindow* g_window;// = 0;

extern std::vector< cMeshObject* > g_vec_pMeshObjects;

// These are no longer being used...
//extern cMeshObject* g_pTheLightMesh;		// or NULL
//extern cMeshObject* g_pTheLightAttenMesh[4];		// or NULL
extern cMeshObject* g_pDebugSphere; 

// And common functions that are called.
// (signatures here)
void LoadObjectsIntoScene(void);
bool LoadModelTypes(GLint shadProgID, std::string &errors);
bool LoadModelTypes(GLint shadProgID, std::vector<std::string> vecModelNames, std::string &errors);

// If true, the spheres around the lights are there
extern bool g_bTurnOnDebugLightSpheres;	// = false
extern bool g_lookAtObject;	// = false
// Use can do this to "select" different lights
extern unsigned int g_SelectedLightID;	// = 0
extern unsigned int g_SelectedObjectID;	// = 0

// Returns NULL if not found
cMeshObject* g_pFindObjectByFriendlyName(std::string name);
cMeshObject* g_pFindObjectByUniqueID(unsigned int uniqueID);

void DoPhysicsIntegrationUpdate(double deltaTime);
void ProcessInputAsync( glm::vec3 &cameraEye, glm::vec3 &cameraTarget, GLFWwindow* &window );

//static was removed because a 'static function' is only visible in that obj file (not outside)
// Once we moved the callback to another cpp file, we had to remove the static.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const unsigned int NUMLIGHTS = 10;
extern std::vector<sLight> g_vecLights;

// When true, the DoPhysicsUpdate is called.
extern bool g_bDoEulerPhysicsUpdate;// = false;		

//Draw an object in space
void DrawObject(cMeshObject* pCurMesh, cShaderManager::cShaderProgram* pShaderProg, cVAOManager* pVAOManager, glm::mat4 matParentModel);

// A more general draw sphere
void DrawDebugSphere(cShaderManager::cShaderProgram* pShaderProgram, glm::vec3 position, glm::vec4 colour, float scale);

//Saving and loading
void save_to_file();
void read_from_file();

#endif

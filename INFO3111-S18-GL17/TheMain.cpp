#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "globalOpenGLStuff.h"

#include "cShaderManager.h"

#include "cVAOManager.h"

//#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>		// for the ply model loader
#include <vector>		// for the ply model loader
#include <sstream>

#include "cMeshObject.h"

#include "commonEngineStuff.h"
// A vector of POINTERS to mesh objects...
//std::vector< cMeshObject* > g_vec_pMeshObjects;
//cMeshObject* g_pTheLightMesh = 0;		// or NULL

#include "cLightHelper.h"


void ShutErDown(void);

GLFWwindow* g_window = 0;



unsigned int g_NumberOfVertices = 0;		// From file
unsigned int g_NumberOfTriangles = 0;		// From file

bool LoadModelTypes(GLint shaderID, std::string &errors);


//void LoadObjectsIntoScene(void);

cShaderManager* g_pTheShaderManager = 0;	// NULL, 0, nullptr

cVAOManager* g_pTheVAOManager = 0;

// When true, the DoPhysicsUpdate is called.
bool g_bDoEulerPhysicsUpdate = false;		


// This will get more involved
struct sLight
{
	sLight()
	{
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
		this->diffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		this->attenConst = 0.0f;
		this->attenLinear = 0.1f;
		this->attenQuad = 0.1f;
		this->UniLoc_Position = -1;
		this->UniLoc_Diffuse = -1;
		this->UniLoc_AttenAndLight = -1;
		return;
	}

	glm::vec3 position;
	glm::vec4 diffuseColour;
	float attenConst;
	float attenLinear;
	float attenQuad;

	GLint UniLoc_Position;
	GLint UniLoc_Diffuse;
	GLint UniLoc_AttenAndLight;

	static const float MAX_ATTENUATION;// = 100.0f;
};
//static 
const float sLight::MAX_ATTENUATION = 100.0f;


const unsigned int NUMLIGHTS = 3;
std::vector<sLight> g_vecLights;

void SetUpTheLights(GLint shaderProgID);
void CopyLightInfoToShader( unsigned int numberOfLightsToCopy );


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

// This one is connected to the regular "keyboard" handler in Winders.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// Is the shift key pressed at the same time?
	if ( mods == GLFW_MOD_SHIFT )	// ONLY shift and nothing else
	{
		if ( ( key == GLFW_KEY_L) && (action == GLFW_PRESS) )
		{
			::g_bTurnOnDebugLightSpheres =  ! ::g_bTurnOnDebugLightSpheres;
		}

		// Select the next light:
		if ( ( key == GLFW_KEY_9 ) && ( action == GLFW_PRESS ) )
		{
			::g_SelectedLightID--;
		}
		if ( ( key == GLFW_KEY_0 ) && ( action == GLFW_PRESS ) )
		{
			::g_SelectedLightID++;
		}
		//Check for wrap around...
		if ( ::g_SelectedLightID >= NUMLIGHTS )
		{
			::g_SelectedLightID = NUMLIGHTS - 1;
		}


	}//if ( mods == GLFW_MOD_SHIFT )

	if ( ( mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT )
	{
		// Shift, and any other modifier, too.
		//DestroyAllHumans();
	}

	// STARTOF: Turn the physics updater on or off
	if ( ( mods & GLFW_MOD_CONTROL ) == GLFW_MOD_CONTROL )
	{
		if ( ( key == GLFW_KEY_P ) && ( action == GLFW_PRESS ) )
		{
			::g_bDoEulerPhysicsUpdate = true;
		}
		else if ( ( key == GLFW_KEY_O ) && ( action == GLFW_PRESS ) )
		{
			::g_bDoEulerPhysicsUpdate = false;
		}
	}// ENDOF: Turn the physics updater on or off

	return;
}

// Processes input (defined below)
void ProcessInput( glm::vec3 &cameraEye, 
                   glm::vec3 &cameraTarget, 
                   GLFWwindow* &window );

int main(void)
{

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	::g_window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (! ::g_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(::g_window, key_callback);
	glfwMakeContextCurrent(::g_window);
	gladLoadGLLoader(( GLADloadproc )glfwGetProcAddress);
	glfwSwapInterval(1);



	// Shader manager thing
	::g_pTheShaderManager = new cShaderManager();

	// A shader "program"
	cShaderManager::cShaderProgram myShader;

	//... which has at least a vertex and a fragment shader
	cShaderManager::cShader vertexShader;	
	cShaderManager::cShader fragmentShader;

	vertexShader.fileName = "vert01.glsl";
	vertexShader.shaderType = cShaderManager::cShader::VERTEX_SHADER;

	fragmentShader.fileName = "frag01.glsl"; 
	fragmentShader.shaderType = cShaderManager::cShader::FRAGMENT_SHADER;

	if ( ! ::g_pTheShaderManager->createProgramFromFile(
			"simpleshader", 
			vertexShader, 
			fragmentShader ) )
	{
		std::cout << "Oh no! Call Coby Briant! The shaders didn't work!" << std::endl;
		// What do you do now? Like seriously, what?
		// Likely just quit
		std::cout << ::g_pTheShaderManager->getLastError() << std::endl;
		return(-1);
	}
	else
	{
		std::cout << "The shader compiled OK." << std::endl;
	}

	GLuint shadProgID
		= ::g_pTheShaderManager->getIDFromFriendlyName("simpleshader");



	::g_pTheShaderManager->useShaderProgram(shadProgID);

	//mvp_location = glGetUniformLocation(shadProgID, "MVP");		// program

	GLint matModel_UniLoc = glGetUniformLocation(shadProgID, "matModel");
	GLint matView_Uniloc = glGetUniformLocation(shadProgID, "matView");
	GLint matProj_Uniloc = glGetUniformLocation(shadProgID, "matProjection");

	// If it returns -1, then it didn't find it.
	GLint meshColourRGBA_UniLoc = glGetUniformLocation(shadProgID, "meshColourRGBA");
	GLint bUse_vColourRGBA_AlphaValue_UniLoc = glGetUniformLocation(shadProgID, "bUse_vColourRGBA_AlphaValue");
	GLint bUseVertexColour_UniLoc = glGetUniformLocation(shadProgID, "bUseVertexColour");

	GLint bDontLightObject_UniLoc = glGetUniformLocation(shadProgID, "bDontLightObject" );
	
	// Shader uniform variables

	// The light values...
	SetUpTheLights(shadProgID);

	::g_vecLights[0].position = glm::vec3( 2.0f, 2.0f, 0.0f );
	::g_vecLights[0].diffuseColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	::g_vecLights[0].attenConst = 0.0f;
	::g_vecLights[0].attenLinear = 0.324f;		// Why this number? It looked nice!
	::g_vecLights[0].attenQuad = 0.0115f;		// Same with this number!

	::g_vecLights[1].position = glm::vec3( 2.0f, 2.0f, 0.0f );
	::g_vecLights[1].diffuseColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	::g_vecLights[1].attenConst = 0.0f;
	::g_vecLights[1].attenLinear = 0.324f;		// Why this number? It looked nice!
	::g_vecLights[1].attenQuad = 0.0115f;		// Same with this number!

	::g_vecLights[2].position = glm::vec3( 2.0f, 2.0f, 0.0f );
	::g_vecLights[2].diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	::g_vecLights[2].attenConst = 0.0f;
	::g_vecLights[2].attenLinear = 0.324f;		// Why this number? It looked nice!
	::g_vecLights[2].attenQuad = 0.0115f;		// Same with this number!

	cLightHelper theLightHelper;


	::g_pTheVAOManager = new cVAOManager();
	
	// Load objects into scene...
	std::string errors;
	if ( ! LoadModelTypes(shadProgID, errors) )
	{
		std::cout << "There were errors loading the model files..." << std::endl;
		std::cout << errors << std::endl;
	}

	LoadObjectsIntoScene();

	// Print out the GL version
	std::cout << glGetString( GL_VERSION ) << std::endl;

	glm::vec3 cameraEye = glm::vec3( 0.0, 0.0, -2.0f ); 
	glm::vec3 cameraTarget = glm::vec3( 0.0f, 0.0f, 0.0f ); 
	glm::vec3 upVector = glm::vec3( 0.0f, 0.1f, 0.0f );


	glEnable( GL_DEPTH_TEST );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );


	// The main "frame" loop, like each time through this
	//	renders the entire scene. (60Hz, 140 Hz)

	double lastTime = glfwGetTime();

	// Used for physics integration update
	double globalLastTimeStamp = glfwGetTime();

	while (!glfwWindowShouldClose(::g_window))
	{
		
		//    ___                    ___ _           _         _   _          _      _            
		//   | __|_ _ _ _  __ _  _  | _ \ |_ _  _ __(_)__ ___ | | | |_ __  __| |__ _| |_ ___ _ _  
		//   | _/ _` | ' \/ _| || | |  _/ ' \ || (_-< / _(_-< | |_| | '_ \/ _` / _` |  _/ -_) '_| 
		//   |_|\__,_|_||_\__|\_, | |_| |_||_\_, /__/_\__/__/  \___/| .__/\__,_\__,_|\__\___|_|   
		//                    |__/           |__/                   |_|                           
		if ( ::g_bDoEulerPhysicsUpdate )
		{
			double currentTime = glfwGetTime();
			// Aka "frame time"
			double deltaTime = currentTime - globalLastTimeStamp;
			DoPhysicsIntegrationUpdate(deltaTime);
			globalLastTimeStamp = currentTime;


			// Move the bunny every 0.5 second
			cMeshObject* pBunny = ::g_pFindObjectByFriendlyName("Bugs");

			// Apply gravity to bunny
			pBunny->acceleration.y = -1.0f; 
			std::cout 
				<< "a = " << pBunny->acceleration.y << ", " 
				<< "v = " << pBunny->velocity.y << std::endl;

			if ( pBunny->pos.y <= -2.0f )
			{
				pBunny->velocity.y = fabs(pBunny->velocity.y);
				pBunny->pos.y = -2.0f;
			}
		}//if ( ::g_bDoEulerPhysicsUpdate )
		


		//    ___          _                                      _   
		//   | _ ) __ _ __(_)__   _ __  _____ _____ _ __  ___ _ _| |_ 
		//   | _ \/ _` (_-< / _| | '  \/ _ \ V / -_) '  \/ -_) ' \  _|
		//   |___/\__,_/__/_\__| |_|_|_\___/\_/\___|_|_|_\___|_||_\__|
		//                                                            
		//if ( pBunny )
		//{
		//	//pBunny->pos.y += 0.01f;		// contin. motion

		//	double currentTime = glfwGetTime();
		//	const double TIME_WE_SHOULD_WAIT = 0.5;		// 500 ms

		//	// How much time has passed? 
		//	if ( (currentTime - lastTime) > TIME_WE_SHOULD_WAIT )
		//	{
		//		// Do the thing
		//		pBunny->pos.y += 0.01f;
		//		lastTime = glfwGetTime();		// "reset" the "last" time

		//	}//if ( (currentTime..

		//}//if ( pBunny )


		float ratio;
		int width, height;
		
		//mat4x4 m, p, mvp;
		glm::mat4 matModel;			// was "m" (also "world")
		glm::mat4 matProjection;	// was "p"
		glm::mat4 matView;			// was "view"

//		glm::mat4 mvp;

		glfwGetFramebufferSize(::g_window, &width, &height);
		ratio = width / ( float )height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		matProjection 
	      = glm::perspective( 0.6f, 
							  ratio, 
							  0.1f,				// Near plane	
							  100000.0f );		// Far plane

		matView = glm::mat4(1.0f);

		// Deal with the keyboard, etc.
		ProcessInput( cameraEye, cameraTarget, ::g_window );




		//matView = glm::lookAt( cameraEye,		// position (3d space)
		//					   cameraTarget,	// looking at
		//					   upVector );		// Up vector

		// Look at Luke's ship
		cMeshObject* pLuke = ::g_pFindObjectByFriendlyName("Luke");
		cMeshObject* pBugs = ::g_pFindObjectByFriendlyName("Bugs");

		matView = glm::lookAt( cameraEye,		// position (3d space)
							   pLuke->pos,		// looking at
							   upVector );		// Up vector


		// **************************************
		if ( ::g_bTurnOnDebugLightSpheres )
		{
			::g_pTheLightAttenMesh[0]->bIsVisible  = 
			::g_pTheLightAttenMesh[1]->bIsVisible = 
			::g_pTheLightAttenMesh[2]->bIsVisible = true;

			::g_pTheLightMesh->bIsVisible = true;


			//// move that light mesh to where the light is at, yo
			::g_pTheLightMesh->pos = ::g_vecLights[::g_SelectedLightID].position;
			::g_pTheLightMesh->scale = 0.01f;


			// Take the other 4 meshes and change the location to where the light is
			::g_pTheLightAttenMesh[::g_SelectedLightID]->pos 
				= ::g_pTheLightAttenMesh[1]->pos
				= ::g_pTheLightAttenMesh[2]->pos
				= ::g_pTheLightAttenMesh[3]->pos = ::g_pTheLightMesh->pos;

			// Draw sphere 0 at 1% brightness
			float distance = theLightHelper.calcApproxDistFromAtten
				( 0.01f,   
				  cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD, 
				  cLightHelper::DEFAULTINFINITEDISTANCE, 
				  ::g_vecLights[::g_SelectedLightID].attenConst,
				  ::g_vecLights[::g_SelectedLightID].attenLinear, 
				  ::g_vecLights[::g_SelectedLightID].attenQuad );

			::g_pTheLightAttenMesh[0]->scale = distance;

			// Draw sphere 0 at 50% brightness
			distance = theLightHelper.calcApproxDistFromAtten
				( 0.50f,   
				  cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD, 
				  cLightHelper::DEFAULTINFINITEDISTANCE, 
				  ::g_vecLights[::g_SelectedLightID].attenConst,
				  ::g_vecLights[::g_SelectedLightID].attenLinear, 
				  ::g_vecLights[::g_SelectedLightID].attenQuad );

			::g_pTheLightAttenMesh[1]->scale = distance;

			// Draw sphere 0 at 90% brightness
			distance = theLightHelper.calcApproxDistFromAtten
				( 0.90f,   
				  cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD, 
				  cLightHelper::DEFAULTINFINITEDISTANCE, 
				  ::g_vecLights[::g_SelectedLightID].attenConst,
				  ::g_vecLights[::g_SelectedLightID].attenLinear, 
				  ::g_vecLights[::g_SelectedLightID].attenQuad );

			::g_pTheLightAttenMesh[2]->scale = distance;

		}//if ( ::g_bTurnOnDebugLightSpheres )
		else
		{
			// Don't Draw the light spheres
			::g_pTheLightAttenMesh[0]->bIsVisible  = 
			::g_pTheLightAttenMesh[1]->bIsVisible = 
			::g_pTheLightAttenMesh[2]->bIsVisible = false;
			::g_pTheLightMesh->bIsVisible = false;
		}//if ( ::g_bTurnOnDebugLightSpheres )


		CopyLightInfoToShader(NUMLIGHTS);


		unsigned int numberOfObjects = 
			static_cast<unsigned int>(::g_vec_pMeshObjects.size() );

		for ( unsigned int meshIndex = 0;
			  meshIndex != numberOfObjects; meshIndex++ )
		{
			cMeshObject* pCurMesh = ::g_vec_pMeshObjects[meshIndex];

			// Is this visible
			if ( ! pCurMesh->bIsVisible )
			{
				// Skip it.
				continue;
			}

			//mat4x4_identity(m);
			matModel = glm::mat4(1.0f);		// because "math"
			


			// Place the object in the world at 'this' location
			glm::mat4 matTranslation 
				= glm::translate( glm::mat4(1.0f), 
			                      glm::vec3( pCurMesh->pos.x,
								             pCurMesh->pos.y, 
								             pCurMesh->pos.z ) );
			matModel = matModel * matTranslation;


			//mat4x4_rotate_Z(m, m, );
			//*************************************
			// ROTATE around Z
			glm::mat4 matRotateZ = glm::rotate( glm::mat4(1.0f), 
											    pCurMesh->orientation.z, // (float) glfwGetTime(), 
											    glm::vec3( 0.0f, 0.0f, 1.0f ) );
			matModel = matModel * matRotateZ;
			//*************************************

			//*************************************
			// ROTATE around Y
			glm::mat4 matRotateY = glm::rotate( glm::mat4(1.0f), 
											 pCurMesh->orientation.y, // (float) glfwGetTime(), 
											 glm::vec3( 0.0f, 1.0f, 0.0f ) );
			matModel = matModel * matRotateY;
			//*************************************

			//*************************************
			// ROTATE around X
			glm::mat4 rotateX = glm::rotate( glm::mat4(1.0f), 
											 pCurMesh->orientation.x, // (float) glfwGetTime(), 
											 glm::vec3( 1.0f, 0.0, 0.0f ) );
			matModel = matModel * rotateX;
			//*************************************

		
			// Set up a scaling matrix
			glm::mat4 matScale = glm::mat4(1.0f);	

			float theScale = pCurMesh->scale;		// 1.0f;		

			matScale = glm::scale( glm::mat4(1.0f), 
								   glm::vec3( theScale, theScale, theScale ) );

			// Apply (multiply) the scaling matrix to 
			// the existing "model" (or "world") matrix
			matModel = matModel * matScale;


			// Also set the colour...
			glUniform4f( meshColourRGBA_UniLoc,			// 'meshColour' in shader
						 pCurMesh->colour.x, 
						 pCurMesh->colour.y,
						 pCurMesh->colour.z,
			             pCurMesh->colour.a );

			// 
			glUniform1f( bUse_vColourRGBA_AlphaValue_UniLoc, GL_FALSE );
			glUniform1f( bUseVertexColour_UniLoc, GL_TRUE );

			if ( pCurMesh->bDontLightObject )
			{
				glUniform1f( bDontLightObject_UniLoc, GL_TRUE );
			}
			else 
			{
				glUniform1f( bDontLightObject_UniLoc, GL_FALSE );
			}


			// Is it wireframe? 
			if ( pCurMesh->isWireframe )
			{	// Yuppers.
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				//glDisable( GL_DEPTH_TEST );
				glDisable( GL_CULL_FACE );
			}
			else
			{
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
				glEnable( GL_DEPTH_TEST );
				glEnable( GL_CULL_FACE );
			}


			glUniformMatrix4fv( matModel_UniLoc, 
							    1, 
							    GL_FALSE, 
								glm::value_ptr(matModel));	

			glUniformMatrix4fv( matView_Uniloc, 
							    1, 
							    GL_FALSE, 
								glm::value_ptr(matView));	

			glUniformMatrix4fv( matProj_Uniloc, 
							    1, 
							    GL_FALSE, 
								glm::value_ptr(matProjection));	


	//		glDrawArrays(GL_TRIANGLES, 0, 3);
	//		glDrawArrays(GL_TRIANGLES, 0, ::g_NumberOfVertsToDraw);

			// Figure out what model we are loading
			sModelDrawInfo modelInfo;
			if ( ::g_pTheVAOManager->FindDrawInfoByModelName( pCurMesh->meshName, modelInfo ) )
			{	// We found something

				// Connect the buffers + vertex attribs to this particular model
				glBindVertexArray( modelInfo.VAO_ID ); 

				// Draw whatever is in that buffer
				glDrawElements( GL_TRIANGLES, 
								modelInfo.numberOfIndices,
								GL_UNSIGNED_INT,			// What type is the index array. Ours is "unsigned int"
								0 );

				// Unbind the buffers + vertex attribs
				glBindVertexArray( 0 );
			}
			// Else we DON'T draw it

		}// for(...

		// Present the "frame buffer" to the screen
		glfwSwapBuffers(::g_window);
		glfwPollEvents();


	} // while (!glfwWindowShouldClose(window))


	ShutErDown();

	glfwDestroyWindow(::g_window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void ProcessInput( glm::vec3 &cameraEye, glm::vec3 &cameraTarget, GLFWwindow* &window )
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

	// 
	//if ( glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS )
	//{
	//	std::cout << "Hey!@" << std::endl;
	//	::g_bTurnOnDebugLightSpheres = false;
	//}


	std::stringstream ssTitle;
	ssTitle << "Light[" << ::g_SelectedLightID << "]: xyz("
		<< ::g_vecLights[::g_SelectedLightID].position.x << ", "
		<< ::g_vecLights[::g_SelectedLightID].position.y << ", "
		<< ::g_vecLights[::g_SelectedLightID].position.z << ") "
		<< "Lin: " 
		<< ::g_vecLights[::g_SelectedLightID].attenLinear << " "
		<< "Quad: " 
		<< ::g_vecLights[::g_SelectedLightID].attenQuad;

	glfwSetWindowTitle( ::g_window, ssTitle.str().c_str() );

	//std::cout << "Camera (xyz): "  
	//		<<cameraEye.x << ", " 
	//		<< cameraEye.y << ", "
	//		<< cameraEye.z << std::endl;
	return;
}




void ShutErDown(void)
{
	// 
	delete ::g_pTheShaderManager;

	::g_pTheVAOManager->ShutDown();
	delete ::g_pTheVAOManager;


	unsigned int vectorSize = (unsigned int)::g_vec_pMeshObjects.size();
	for ( unsigned int index = 0; index != vectorSize; index++ )
	{
		delete ::g_vec_pMeshObjects[index];
	}


	return;
}

bool LoadModelTypes(GLint shadProgID, std::string &errors)
{
	std::stringstream ssError;

	bool bAllGood = true;

//	"bun_zipper_res2_xyz.ply", "ssj100_xyz.ply", "building_xyz.ply"
	sModelDrawInfo bunny;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "bun_zipper_res2_xyz_n_rgba_uv.ply", bunny, shadProgID ) )
	{
		ssError << "ERROR: bun_zipper_res2_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
		bAllGood = false;
	}

	sModelDrawInfo cow;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "cow_xyz_n_rgba_uv.ply", cow, shadProgID ) )
	{
		ssError << "ERROR: cow_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
		bAllGood = false;
	}	

	sModelDrawInfo airplane;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "ssj100_xyz_n_rgba_uv.ply", airplane, shadProgID ) )
	{
		ssError << "ERROR: ssj100_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
		bAllGood = false;
	}

	sModelDrawInfo arena;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "free_arena_ASCII_xyz_n_rgba_uv.ply", arena, shadProgID ) )
	{
		ssError << "ERROR: free_arena_ASCII_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
		bAllGood = false;
	}

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

	sModelDrawInfo UseTheForceLuke;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "X-Wing_Attack_(33569 faces)_xyz_n_rgba_uv.ply", UseTheForceLuke, shadProgID ) )
	{
		ssError << "ERROR: X-Wing_Attack_(33569 faces)_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
		bAllGood = false;
	}

	errors = ssError.str();

	return bAllGood;
}

//struct sLight
//{
//	glm::vec3 position;
//	float attenLinear;
//	float attenConst;
//	float attenQuad;
//
//	GLint UniLoc_Position;
//	GLint UniLoc_AttenAndLight;
//};
//
//const unsigned int NUMLIGHTS = 3;
//std::vector<sLight> g_vecLights;

void SetUpTheLights(GLint shaderProgID)
{
	for ( int count = 0; count != NUMLIGHTS; count++ )
	{
		sLight tempLight;
		// Note that the constructor set a bunch of defaults
		::g_vecLights.push_back( tempLight );
	}

	// Set the #0 light to white
	::g_vecLights[0].diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


	::g_vecLights[0].UniLoc_Position = glGetUniformLocation( shaderProgID, "theLights[0].Position" );
	::g_vecLights[0].UniLoc_Diffuse = glGetUniformLocation( shaderProgID, "theLights[0].Diffuse" );
	::g_vecLights[0].UniLoc_AttenAndLight = glGetUniformLocation( shaderProgID, "theLights[0].AttenAndType" );

	::g_vecLights[1].UniLoc_Position = glGetUniformLocation( shaderProgID, "theLights[1].Position" );
	::g_vecLights[1].UniLoc_Diffuse = glGetUniformLocation( shaderProgID, "theLights[1].Diffuse" );
	::g_vecLights[1].UniLoc_AttenAndLight = glGetUniformLocation( shaderProgID, "theLights[1].AttenAndType" );

	::g_vecLights[2].UniLoc_Position = glGetUniformLocation( shaderProgID, "theLights[2].Position" );
	::g_vecLights[2].UniLoc_Diffuse = glGetUniformLocation( shaderProgID, "theLights[2].Diffuse" );
	::g_vecLights[2].UniLoc_AttenAndLight = glGetUniformLocation( shaderProgID, "theLights[2].AttenAndType" );

	return;
}

void CopyLightInfoToShader( unsigned int numberOfLightsToCopy )
{
	for ( unsigned int index = 0; index != numberOfLightsToCopy; index++ )
	{
		sLight &curLight = ::g_vecLights[index];

		glUniform3f( curLight.UniLoc_Position, curLight.position.x, curLight.position.y, curLight.position.z );

		glUniform4f( curLight.UniLoc_Diffuse, 
					 curLight.diffuseColour.r, curLight.diffuseColour.g, curLight.diffuseColour.b, 
					 curLight.diffuseColour.a );

		glUniform4f( curLight.UniLoc_AttenAndLight, 
					 curLight.attenConst, 
					 curLight.attenLinear, 
					 curLight.attenQuad, 
					 1.0f );
	}//for ( unsigned int index = ...

	return;
}


void DoPhysicsIntegrationUpdate(double deltaTime)
{
	// Check if too big
	const double BIGGEST_TIME_STEP_ALLOWED = 0.1f;	// 100 ms is pretty big...
	
	if ( deltaTime > BIGGEST_TIME_STEP_ALLOWED )
	{
		deltaTime = BIGGEST_TIME_STEP_ALLOWED;
	}



	unsigned int numberOfObjects = (unsigned int)::g_vec_pMeshObjects.size();
	for ( unsigned int meshIndex = 0;
		meshIndex != numberOfObjects; meshIndex++ )
	{
		cMeshObject* pCurMesh = ::g_vec_pMeshObjects[meshIndex];

		if ( pCurMesh->friendlyName != "Bugs" )
		{
			continue;
		}

		// new position = old postion + (velocity * time)
		// new velocity = old velocity + (acceleration * time)

		pCurMesh->velocity += (float)deltaTime * pCurMesh->acceleration;

		pCurMesh->pos += (float)deltaTime * pCurMesh->velocity;


		pCurMesh = NULL;
	}


	return;
}
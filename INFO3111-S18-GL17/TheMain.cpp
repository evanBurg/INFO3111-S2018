#include "globalStuff.h"
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include "cShaderManager.h"
#include "cVAOManager.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>		// for the ply model loader
#include <vector>		// for the ply model loader
#include <sstream>
#include "cMeshObject.h"
#include "sLight.h"
#include "cLightHelper.h"

//Function Declariations
void ShutErDown(void);
bool LoadModelTypes(GLint shaderID, std::string &errors);
void SetUpTheLights(GLint shaderProgID);
void CopyLightInfoToShader(unsigned int numberOfLightsToCopy);
void DrawDebugLightSpheres(cShaderManager::cShaderProgram* pShaderProgram);
void DrawDebugLightSpheres2(cShaderManager::cShaderProgram* pShaderProgram);

//Global Variables
GLFWwindow* g_window = 0;
unsigned int g_NumberOfVertices = 0;		// From file
unsigned int g_NumberOfTriangles = 0;		// From file
cShaderManager* g_pTheShaderManager = 0;	// NULL, 0, nullptr
cVAOManager* g_pTheVAOManager = 0;


/**********
error_callback()
Used for printing errors to the console
@param error Integer error code
@param description The error message
**********/
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

/**********
main(void)
The main code of the loop with the while loop for rendering each frame
**********/
int main(void)
{
	//Set the error callback so that if an error happens in runtime, it will call this function
	glfwSetErrorCallback(error_callback);

	//Try to start the program by calling glfwInit(), it will return false if it cannot start and exit the program.
	if (!glfwInit())
		exit(EXIT_FAILURE);

	//Set the minor and major GLFW versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//Set the window properties
	::g_window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	//If the window didnt get initialized by the line above close the program
	if (!::g_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//Set the key callback function. (Now in key_callback_GLFW.cpp)
	glfwSetKeyCallback(::g_window, key_callback);
	glfwMakeContextCurrent(::g_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	//Create the shader manager used to hold the shader program, the individual shader
	//as well as the uniform locations of the shader variables
	::g_pTheShaderManager = new cShaderManager();

	// A shader "program"
	cShaderManager::cShaderProgram myShader;

	//... which has at least a vertex and a fragment shader
	cShaderManager::cShader vertexShader;
	cShaderManager::cShader fragmentShader;

	//Specify the file names and types
	vertexShader.fileName = "vert01.glsl";
	vertexShader.shaderType = cShaderManager::cShader::VERTEX_SHADER;

	fragmentShader.fileName = "frag01.glsl";
	fragmentShader.shaderType = cShaderManager::cShader::FRAGMENT_SHADER;

	//Attempt to create a shader program
	//The 'FromFile' part of the method name refers to loading the vertex and fragment shaders from file, 'simpleshader' is the name of these combined
	if (!::g_pTheShaderManager->createProgramFromFile(
		"simpleshader",
		vertexShader,
		fragmentShader))
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

	//Get the shader program ID, just like shader variables you can only access the program by ID
	GLuint shadProgID = ::g_pTheShaderManager->getIDFromFriendlyName("simpleshader");
	::g_pTheShaderManager->useShaderProgram(shadProgID);

	// Get the shader by name (so we can load the unforms)
	cShaderManager::cShaderProgram* pShader = ::g_pTheShaderManager->pGetShaderProgramFromFriendlyName("simpleshader");

	//Get the uniform locations and save them in the shader object
	pShader->LoadUniformLocation("bDontLightObject");
	pShader->LoadUniformLocation("bUseVertexColour");
	pShader->LoadUniformLocation("bUse_vColourRGBA_AlphaValue");
	pShader->LoadUniformLocation("meshColourRGBA");
	pShader->LoadUniformLocation("matProjection");
	pShader->LoadUniformLocation("matView");
	pShader->LoadUniformLocation("matModel");

	// The light values...
	SetUpTheLights(shadProgID);

	// Load objects into scene...
	::g_pTheVAOManager = new cVAOManager();
	std::vector<std::string> vecModelFilesToLoad;
	vecModelFilesToLoad.push_back("bun_zipper_res2_xyz_n_rgba_uv.ply");
	vecModelFilesToLoad.push_back("cow_xyz_n_rgba_uv.ply");
	vecModelFilesToLoad.push_back("ssj100_xyz_n_rgba_uv.ply");
	vecModelFilesToLoad.push_back("free_arena_ASCII_xyz_n_rgba_uv.ply");
	vecModelFilesToLoad.push_back("CrappyTerrain_xyz_n_rgba_uv.ply");
	vecModelFilesToLoad.push_back("isosphere_smooth_xyz_n_rgba_uv.ply");
	vecModelFilesToLoad.push_back("X-Wing_Attack_(33569 faces)_xyz_n_rgba_uv.ply");

	//Try loading all the specified model types into the VAO manager
	//LoadModelTypes is now in the 'ObjectLoadingStuff.cpp'
	std::string errors;
	if (!LoadModelTypes(shadProgID, vecModelFilesToLoad, errors))
	{
		std::cout << "There were errors loading the model files..." << std::endl;
		std::cout << errors << std::endl;
	}

	//Populate the global g_vecMeshObjects vector with each mesh properties
	LoadObjectsIntoScene();

	// Print out the GL version
	std::cout << glGetString(GL_VERSION) << std::endl;

	//Set the camera properties
	glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -2.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 0.1f, 0.0f);

	//Various openGL flags to render things correctly
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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
		if (::g_bDoEulerPhysicsUpdate)
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

			if (pBunny->pos.y <= -2.0f)
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
		//glm::mat4 matModel;			// was "m" (also "world")
		glm::mat4 matProjection;	// was "p"
		glm::mat4 matView;			// was "view"

		glfwGetFramebufferSize(::g_window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

												//Near Plane, Far plane
		matProjection = glm::perspective(0.6f, ratio, 0.1f,	100000.0f);

		matView = glm::mat4(1.0f);

		// Deal with the keyboard, etc.
		ProcessInputAsync(cameraEye, cameraTarget, ::g_window);

							//Position,  Looking At, Up Direction
		matView = glm::lookAt(cameraEye, cameraTarget,	upVector);

		// Start of the objects in the scene... 
		cShaderManager::cShaderProgram* pShaderProgram = ::g_pTheShaderManager->pGetShaderProgramFromFriendlyName("simpleshader");

		// **************************************
		// MOVED: if ( ::g_bTurnOnDebugLightSpheres )
		//DrawDebugLightSpheres(pShaderProgram);
		DrawDebugLightSpheres2(pShaderProgram);

		CopyLightInfoToShader(NUMLIGHTS);

		glUniformMatrix4fv(pShaderProgram->getUniformID_From_Name("matView"), 1, GL_FALSE, glm::value_ptr(matView));

		glUniformMatrix4fv(pShaderProgram->getUniformID_From_Name("matProjection"),	1, GL_FALSE, glm::value_ptr(matProjection));


		unsigned int numberOfObjects =
			static_cast<unsigned int>(::g_vec_pMeshObjects.size());

		for (unsigned int meshIndex = 0;
			meshIndex != numberOfObjects; meshIndex++)
		{
			cMeshObject* pCurMesh = ::g_vec_pMeshObjects[meshIndex];

			// And.... draw the mesh:

			glm::mat4 matParentModel = glm::mat4(1.0f);


			DrawObject(pCurMesh, pShaderProgram, ::g_pTheVAOManager, matParentModel);

		}// for(...
		 // At this point, all the objects in the "main scene" 
		 // are drawn... 

		 // Present the "frame buffer" to the screen
		glfwSwapBuffers(::g_window);
		glfwPollEvents();


	} // while (!glfwWindowShouldClose(window))


	ShutErDown();

	glfwDestroyWindow(::g_window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

/**********
ShutErDown(void)
Used to free up memory after closing the window
**********/
void ShutErDown(void)
{
	// 
	delete ::g_pTheShaderManager;

	::g_pTheVAOManager->ShutDown();
	delete ::g_pTheVAOManager;


	unsigned int vectorSize = (unsigned int)::g_vec_pMeshObjects.size();
	for (unsigned int index = 0; index != vectorSize; index++)
	{
		delete ::g_vec_pMeshObjects[index];
	}


	return;
}


/**********
SetUpTheLights(shaderProgId)
Populates the global light vector with empty lights
Gets the uniform location of the light variables in the shader
@param shaderProgId The ID of the shader program
**********/
void SetUpTheLights(GLint shaderProgID)
{
	for (int count = 0; count != NUMLIGHTS; count++)
	{
		sLight tempLight;
		// Note that the constructor set a bunch of defaults
		::g_vecLights.push_back(tempLight);
	}

	// Set the #0 light to white
	::g_vecLights[0].diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Load all the uniform variables for these lights:
	for (int count = 0; count != NUMLIGHTS; count++)
	{
		std::string positionTemp = "theLights[";
		positionTemp += std::to_string(count);
		positionTemp += "].Position";
		const GLchar * position = positionTemp.c_str();

		std::string diffuseTemp = "theLights[";
		diffuseTemp += std::to_string(count);
		diffuseTemp += "].Diffuse";
		const GLchar * diffuse = diffuseTemp.c_str();

		std::string attenTemp = "theLights[";
		attenTemp += std::to_string(count);
		attenTemp += "].AttenAndType";
		const GLchar * atten = attenTemp.c_str();

		::g_vecLights[count].UniLoc_Position = glGetUniformLocation(shaderProgID, position);
		::g_vecLights[count].UniLoc_Diffuse = glGetUniformLocation(shaderProgID, diffuse);
		::g_vecLights[count].UniLoc_AttenAndLight = glGetUniformLocation(shaderProgID, atten);
	}


	// Set up the light values, too
	::g_vecLights[0].position = glm::vec3(2.0f, 2.0f, 0.0f);
	::g_vecLights[0].diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_vecLights[0].attenConst = 0.0f;
	::g_vecLights[0].attenLinear = 0.324f;		// Why this number? It looked nice!
	::g_vecLights[0].attenQuad = 0.0115f;		// Same with this number!

	::g_vecLights[1].position = glm::vec3(2.0f, 2.0f, 0.0f);
	::g_vecLights[1].diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_vecLights[1].attenConst = 0.0f;
	::g_vecLights[1].attenLinear = 0.324f;		// Why this number? It looked nice!
	::g_vecLights[1].attenQuad = 0.0115f;		// Same with this number!

	::g_vecLights[2].position = glm::vec3(2.0f, 2.0f, 0.0f);
	::g_vecLights[2].diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_vecLights[2].attenConst = 0.0f;
	::g_vecLights[2].attenLinear = 0.324f;		// Why this number? It looked nice!
	::g_vecLights[2].attenQuad = 0.0115f;		// Same with this number!

	return;
}

/**********
CopyLightInfoToShader
Called every frame to set the light variables in the shader
@param numberOfLightsToCopy How many lights we are using in the shader
**********/
void CopyLightInfoToShader(unsigned int numberOfLightsToCopy)
{
	for (unsigned int index = 0; index != numberOfLightsToCopy; index++)
	{
		sLight &curLight = ::g_vecLights[index];

		glUniform3f(curLight.UniLoc_Position, curLight.position.x, curLight.position.y, curLight.position.z);

		glUniform4f(curLight.UniLoc_Diffuse, curLight.diffuseColour.r, curLight.diffuseColour.g, curLight.diffuseColour.b, curLight.diffuseColour.a);

		glUniform4f(curLight.UniLoc_AttenAndLight, curLight.attenConst, curLight.attenLinear, curLight.attenQuad, 1.0f);
	}//for ( unsigned int index = ...

	return;
}

/**********
DoPhysicsIntegrationUpdate
Create an animation using some physics values
**********/
void DoPhysicsIntegrationUpdate(double deltaTime)
{
	// Check if too big
	const double BIGGEST_TIME_STEP_ALLOWED = 0.1f;	// 100 ms is pretty big...

	if (deltaTime > BIGGEST_TIME_STEP_ALLOWED)
	{
		deltaTime = BIGGEST_TIME_STEP_ALLOWED;
	}

	unsigned int numberOfObjects = (unsigned int)::g_vec_pMeshObjects.size();
	for (unsigned int meshIndex = 0;
		meshIndex != numberOfObjects; meshIndex++)
	{
		cMeshObject* pCurMesh = ::g_vec_pMeshObjects[meshIndex];

		if (pCurMesh->friendlyName != "Bugs")
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

/**********
DrawDebugSphere
Used to draw a debug sphere at a specified location with a specified scale and colour
@param pShaderProgram The shader program is used for the DrawObject call
@param position The XYZ of the sphere
@param colour The colour of the sphere
@param scale The size of the sphere
**********/
void DrawDebugSphere(cShaderManager::cShaderProgram* pShaderProgram, glm::vec3 position, glm::vec4 colour, float scale)
{
	bool bOldState = ::g_pDebugSphere->bIsVisible;

	::g_pDebugSphere->pos = position;
	::g_pDebugSphere->bIsVisible = true;
	::g_pDebugSphere->bDontLightObject = true;
	::g_pDebugSphere->bUseColourAlphaValue = true;
	::g_pDebugSphere->isWireframe = true;
	::g_pDebugSphere->colourSource = cMeshObject::USE_OBJECT_COLOUR;


	// Draw centre of light (the position)
	::g_pDebugSphere->scale = scale;
	::g_pDebugSphere->colour = colour;
	DrawObject(::g_pDebugSphere, pShaderProgram, ::g_pTheVAOManager, glm::mat4(1.0f));


	// Restore it
	::g_pDebugSphere->bIsVisible = bOldState;

	return;
}

/**********
DrawDebugLightSpheres2
This is used to draw light spheres at your currently selected light,
it automatically gets the location and sets the colour and scale for you.
It calls the main DrawDebugSphere function
**********/
void DrawDebugLightSpheres2(cShaderManager::cShaderProgram* pShaderProgram)
{
	// Static so it's created one (not over and over again)
	// (before main is called, not that it matters with this object)
	static cLightHelper theLightHelper;


	// **************************************
	if (::g_bTurnOnDebugLightSpheres)
	{
		// Draw centre of light (the position)
		DrawDebugSphere(
			pShaderProgram,
			::g_vecLights[::g_SelectedLightID].position,
			glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::WHITE), 1.0f),
			0.01f
		);

		// Draw sphere 0 at 1% brightness
		float distance = theLightHelper.calcApproxDistFromAtten
		
		(0.01f,
			cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD,
			cLightHelper::DEFAULTINFINITEDISTANCE,
			::g_vecLights[::g_SelectedLightID].attenConst,
			::g_vecLights[::g_SelectedLightID].attenLinear,
			::g_vecLights[::g_SelectedLightID].attenQuad
		);
		DrawDebugSphere(
			pShaderProgram,
			::g_vecLights[::g_SelectedLightID].position,
			glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::DARK_GRAY), 1.0f),
			distance
		);

		// Draw sphere 0 at 50% brightness
		distance = theLightHelper.calcApproxDistFromAtten
		(
			0.50f,
			cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD,
			cLightHelper::DEFAULTINFINITEDISTANCE,
			::g_vecLights[::g_SelectedLightID].attenConst,
			::g_vecLights[::g_SelectedLightID].attenLinear,
			::g_vecLights[::g_SelectedLightID].attenQuad
		);
		DrawDebugSphere(
			pShaderProgram,
			::g_vecLights[::g_SelectedLightID].position,
			glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::LIGHT_SALMON), 1.0f),
			distance
		);

		// Draw sphere 0 at 90% brightness
		distance = theLightHelper.calcApproxDistFromAtten
		(
			0.90f,
			cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD,
			cLightHelper::DEFAULTINFINITEDISTANCE,
			::g_vecLights[::g_SelectedLightID].attenConst,
			::g_vecLights[::g_SelectedLightID].attenLinear,
			::g_vecLights[::g_SelectedLightID].attenQuad
		);
		DrawDebugSphere(
			pShaderProgram, 
			::g_vecLights[::g_SelectedLightID].position, 
			glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::CYAN), 1.0f), 
			distance
		);

	}// if ( ::g_bTurnOnDebugLightSpheres )


	return;
}

/**********
DrawDebugLightSpheres
This is used to draw light spheres at your currently selected light,
it automatically gets the location and sets the colour and scale for you.
It DOES NOT call the main DrawDebugSphere function instead calling DrawObject itself
**********/
void DrawDebugLightSpheres(cShaderManager::cShaderProgram* pShaderProgram)
{
	// Static so it's created one (not over and over again)
	// (before main is called, not that it matters with this object)
	static cLightHelper theLightHelper;


	// **************************************
	if (::g_bTurnOnDebugLightSpheres)
	{
		::g_pDebugSphere->pos = ::g_vecLights[::g_SelectedLightID].position;
		::g_pDebugSphere->bIsVisible = true;
		::g_pDebugSphere->bDontLightObject = true;
		::g_pDebugSphere->bUseColourAlphaValue = true;
		::g_pDebugSphere->isWireframe = true;
		::g_pDebugSphere->colourSource = cMeshObject::USE_OBJECT_COLOUR;

		// Draw centre of light (the position)
		::g_pDebugSphere->scale = 0.01f;
		::g_pDebugSphere->colour = glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::WHITE), 1.0f);
		DrawObject(::g_pDebugSphere, pShaderProgram, ::g_pTheVAOManager, glm::mat4(1.0f));

		// Draw sphere 0 at 1% brightness
		float distance = theLightHelper.calcApproxDistFromAtten
		(
			0.01f,
			cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD,
			cLightHelper::DEFAULTINFINITEDISTANCE,
			::g_vecLights[::g_SelectedLightID].attenConst,
			::g_vecLights[::g_SelectedLightID].attenLinear,
			::g_vecLights[::g_SelectedLightID].attenQuad
		);
		::g_pDebugSphere->scale = distance;
		::g_pDebugSphere->colour = glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::DARK_GRAY), 1.0f);
		DrawObject(::g_pDebugSphere, pShaderProgram, ::g_pTheVAOManager, glm::mat4(1.0f));

		// Draw sphere 0 at 50% brightness
		distance = theLightHelper.calcApproxDistFromAtten
		(
			0.50f,
			cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD,
			cLightHelper::DEFAULTINFINITEDISTANCE,
			::g_vecLights[::g_SelectedLightID].attenConst,
			::g_vecLights[::g_SelectedLightID].attenLinear,
			::g_vecLights[::g_SelectedLightID].attenQuad
		);
		::g_pDebugSphere->scale = distance;
		::g_pDebugSphere->colour = glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::LIGHT_SALMON), 1.0f);
		DrawObject(::g_pDebugSphere, pShaderProgram, ::g_pTheVAOManager, glm::mat4(1.0f));

		// Draw sphere 0 at 90% brightness
		distance = theLightHelper.calcApproxDistFromAtten
		(
			0.90f,
			cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD,
			cLightHelper::DEFAULTINFINITEDISTANCE,
			::g_vecLights[::g_SelectedLightID].attenConst,
			::g_vecLights[::g_SelectedLightID].attenLinear,
			::g_vecLights[::g_SelectedLightID].attenQuad
		);
		::g_pDebugSphere->scale = distance;
		::g_pDebugSphere->colour = glm::vec4(CGLColourHelper::getInstance()->getColourRGB(CGLColourHelper::CYAN), 1.0f);
		DrawObject(::g_pDebugSphere, pShaderProgram, ::g_pTheVAOManager, glm::mat4(1.0f));


	}//if ( ::g_bTurnOnDebugLightSpheres )
	return;
}
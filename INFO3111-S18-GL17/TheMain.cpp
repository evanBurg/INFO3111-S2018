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

void ShutErDown(void);

GLFWwindow* g_window = 0;

cMeshObject* g_pTheLightMesh = 0;		// or NULL


unsigned int g_NumberOfVertices = 0;		// From file
unsigned int g_NumberOfTriangles = 0;		// From file

bool LoadModelTypes(GLint shaderID, std::string &errors);

// A vector of POINTERS to mesh objects...
std::vector< cMeshObject* > g_vec_pMeshObjects;
void LoadObjectsIntoScene(void);

cShaderManager* g_pTheShaderManager = 0;	// NULL, 0, nullptr

cVAOManager* g_pTheVAOManager = 0;


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
	float attenLinear;
	float attenConst;
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
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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
	::g_vecLights[0].diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_vecLights[0].attenConst = 0.0f;
	::g_vecLights[0].attenLinear = 0.324f;		// Why this number? It looked nice!
	::g_vecLights[0].attenQuad = 0.0115f;		// Same with this number!


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

	while (!glfwWindowShouldClose(::g_window))
	{
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




		matView = glm::lookAt( cameraEye,		// position (3d space)
							   cameraTarget,	// looking at
							   upVector );		// Up vector



		//// move that light mesh to where the light is at, yo
		::g_pTheLightMesh->pos = ::g_vecLights[0].position;

		CopyLightInfoToShader(NUMLIGHTS);


		unsigned int numberOfObjects = 
			static_cast<unsigned int>(::g_vec_pMeshObjects.size() );

		for ( unsigned int meshIndex = 0;
			  meshIndex != numberOfObjects; meshIndex++ )
		{
			cMeshObject* pCurMesh = ::g_vec_pMeshObjects[meshIndex];


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
				glDisable( GL_DEPTH_TEST );
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
	float cameraSpeed = 0.01f; 

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
		if ( ::g_vecLights[0].attenLinear <= 0.0f )	 
		{ 
			::g_vecLights[0].attenLinear = 0.001f;		// Make it a tiny value
		}
		else
		{
			::g_vecLights[0].attenLinear *= 1.01f;		// + 1%
			if ( ::g_vecLights[0].attenLinear >= sLight::MAX_ATTENUATION )
			{
				::g_vecLights[0].attenLinear = sLight::MAX_ATTENUATION;		
			}
		}
	}
	if ( glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS )
	{	// Decrease quadratic atten by 1%
		::g_vecLights[0].attenQuad *= 0.99f;			// +1%
	}

	if ( glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS )
	{	// Increase quadratic atten by 1%
		if ( ::g_vecLights[0].attenQuad <= 0.0f )	 
		{ 
			::g_vecLights[0].attenQuad = 0.0001f;		// Make it a tiny value
		}
		else
		{
			::g_vecLights[0].attenQuad *= 1.01f;		// + 1%
			if ( ::g_vecLights[0].attenQuad >= sLight::MAX_ATTENUATION )
			{
				::g_vecLights[0].attenQuad = sLight::MAX_ATTENUATION;		
			}
		}
	}	

	// Also move the light around
	if ( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS )
	{
		::g_vecLights[0].position.z += cameraSpeed;		
	}
	if ( glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS )
	{
		::g_vecLights[0].position.z -= cameraSpeed;		
	}		
	if ( glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS )
	{
		::g_vecLights[0].position.x -= cameraSpeed;		
	}	
	if ( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS )
	{
		::g_vecLights[0].position.x += cameraSpeed;		
	}		
	if ( glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS )
	{
		::g_vecLights[0].position.y += cameraSpeed;		
	}		
	if ( glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS )
	{
		::g_vecLights[0].position.y -= cameraSpeed;		
	}		


	std::stringstream ssTitle;
	ssTitle << "Light[0]: xyz("
		<< ::g_vecLights[0].position.x << ", "
		<< ::g_vecLights[0].position.y << ", "
		<< ::g_vecLights[0].position.z << ") "
		<< "Lin: " 
		<< ::g_vecLights[0].attenLinear << " "
		<< "Quad: " 
		<< ::g_vecLights[0].attenQuad;

	glfwSetWindowTitle( ::g_window, ssTitle.str().c_str() );

	//std::cout << "Camera (xyz): "  
	//		<<cameraEye.x << ", " 
	//		<< cameraEye.y << ", "
	//		<< cameraEye.z << std::endl;
	return;
}


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
		::g_pTheLightMesh->scale = 0.1f;
		::g_pTheLightMesh->isWireframe = false;

		::g_pTheLightMesh->bDontLightObject = true;

		::g_vec_pMeshObjects.push_back( ::g_pTheLightMesh );
	}	


	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "CrappyTerrain_xyz_n_rgba_uv.ply";

		pTemp->pos = glm::vec3( 0.0f, -10.0f, 0.0f );
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
		pTemp->scale = 0.05f;
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
		pTemp->scale = 0.1f;
		pTemp->isWireframe = true;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "bun_zipper_res2_xyz_n_rgba_uv.ply";

		pTemp->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 1.0f,
								   1.0f, 
									1.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 2.0f;
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
		pTemp->orientation.y = glm::pi<float>() * 0.5f;		// YOLO, right? 

		pTemp->pos = glm::vec3( -1.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 
									49.0f/255.0f,
								   205.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 1.5f;
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
		
		pTemp->scale = 1.0f/40.2828f;

		// Now my model 1.0 unit in size (-1 to 1)

		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}


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
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "isosphere_smooth_xyz_n_rgba_uv.ply", terrain, shadProgID ) )
	{
		ssError << "ERROR: isosphere_smooth_xyz_n_rgba_uv.ply wasn't loaded" << std::endl;
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



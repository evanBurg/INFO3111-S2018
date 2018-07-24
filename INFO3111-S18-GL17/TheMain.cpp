/**
* EVAN BURGESS
* 0781910
* INFO-3111
* JULY 23rd, 2018
*/


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

#include "cMeshObject.h"

void ShutErDown(void);

cMeshObject* g_pTheLightMesh = 0;		// or NULL


//static const struct
//{
//	float x, y;
//	float r, g, b;
//} vertices[3] =
//{
//	{ -0.6f, -0.4f,		0.5f, 0.1f, 0.2f },
//	{ 0.6f, -0.4f,		0.f, 1.f, 0.f },
//	{ 0.f,  0.6f,		0.f, 1.f, 0.f }
//};

//struct sVert
//{
//	float x, y, z;		// added "z"
//	float r, g, b;
//};

//sVert vertices[3] = 
//{
//	{ -0.6f, -0.4f, 0.0f,		0.5f, 0.1f, 0.2f },
//	{ 0.6f, -0.4f, 0.0f, 		0.f, 1.f, 0.f },
//	{ 0.f,  0.6f, 0.0f,			0.f, 1.f, 0.f }
//};
// Here's our heap based vertex array:
//sVert* pVertices = 0;						//  = new sVert[10000];
//unsigned int g_NumberOfVertsToDraw = 0;		// To draw (on GPU)


int selectedLight = 0;

unsigned int g_NumberOfVertices = 0;		// From file
unsigned int g_NumberOfTriangles = 0;		// From file


//// Function signature for loading the ply model
//// Will load model and place into pVertices array
//// (overwriting whatever was there)
//void LoadTheModel(std::string fileName);

// A vector of POINTERS to mesh objects...
std::vector< cMeshObject* > g_vec_pMeshObjects;
void LoadObjectsIntoScene(void);

cShaderManager* g_pTheShaderManager = 0;	// NULL, 0, nullptr

cVAOManager* g_pTheVAOManager = 0;

//static const char* vertex_shader_text =
//"uniform mat4 MVP;\n"
//"uniform vec3 meshColour; \n"
//"attribute vec3 vCol;\n"		// float r, g, b;
//"attribute vec3 vPos;\n"		// float x, y, z;
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    vec3 newVertex = vPos;				\n"
//"    gl_Position = MVP * vec4(newVertex, 1.0);\n"
//"    color = meshColour;\n"				// color = vCol;
//"    color *= 0.01f;\n"
//"    color += vCol;\n"
//"}\n";

//static const char* fragment_shader_text =
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(color, 1.0);\n"
//"}\n";

// This will get more involved
struct sLight
{
	glm::vec3 position;
	float attenLinear;
	float attenConst;
	float attenQuad;
	glm::vec3 color;
};

sLight g_L1;
sLight g_L2;
sLight g_L3;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
		if(selectedLight+1 <= 2)
			selectedLight += 1;

		::g_vec_pMeshObjects[0]->colour = glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
		::g_vec_pMeshObjects[1]->colour = glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
		::g_vec_pMeshObjects[2]->colour = glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
		::g_vec_pMeshObjects[selectedLight]->colour = glm::vec4(244.0f / 255.0f, 51.0f / 255.0f, 255.0f / 255.0f, 1.0f);
	}
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
		if (selectedLight - 1 > -1)
			selectedLight -= 1;

		::g_vec_pMeshObjects[0]->colour = glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
		::g_vec_pMeshObjects[1]->colour = glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
		::g_vec_pMeshObjects[2]->colour = glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
		::g_vec_pMeshObjects[selectedLight]->colour = glm::vec4(244.0f / 255.0f, 51.0f / 255.0f, 255.0f / 255.0f, 1.0f);
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		::g_vec_pMeshObjects.clear();
		LoadObjectsIntoScene();
		::g_L1.position = glm::vec3(5.0f, 3.0f, 0.0f);
		::g_L1.attenConst = 0.0f;		// NO attenuation
		::g_L1.attenLinear = 0.1f;		// NO attenuation
		::g_L1.attenQuad = 0.0f;		// NO attenuation
		::g_L1.color = glm::vec3(1.0f, 0.0f, 0.0f);
		::g_L2.position = glm::vec3(-5.0f, 3.0f, 0.0f);
		::g_L2.attenConst = 0.0f;		// NO attenuation
		::g_L2.attenLinear = 0.1f;		// NO attenuation
		::g_L2.attenQuad = 0.0f;		// NO attenuation
		::g_L2.color = glm::vec3(0.0f, 1.0f, 0.0f);
		::g_L3.position = glm::vec3(-5.0f, 0.0f, 5.0f);
		::g_L3.attenConst = 0.0f;		// NO attenuation
		::g_L3.attenLinear = 0.1f;		// NO attenuation
		::g_L3.attenQuad = 0.0f;		// NO attenuation
		::g_L3.color = glm::vec3(0.0f, 0.0f, 1.0f);
	}
}

// Processes input (defined below)
void ProcessInput( glm::vec3 &cameraEye, 
                   glm::vec3 &cameraTarget, 
                   GLFWwindow* &window );

int main(void)
{
	GLFWwindow* window;
	//GLuint vertex_buffer, vertex_shader, fragment_shader;
	//GLuint program;
	GLint mvp_location; 
	//GLint vpos_location;
	//GLint vcol_location;

	GLint meshColour_UniLoc = -1; 

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader(( GLADloadproc )glfwGetProcAddress);
	glfwSwapInterval(1);

//	LoadTheModel("bun_zipper_res2_xyz.ply");
//	LoadTheModel("ssj100_xyz.ply");
//	LoadTheModel("building_xyz.ply");

	// Load objects into scene...
	LoadObjectsIntoScene();

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

//	glUseProgram( 3 );
	GLuint shadProgID
		= ::g_pTheShaderManager->getIDFromFriendlyName("simpleshader");

	::g_pTheShaderManager->useShaderProgram(shadProgID);

	//vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	//glCompileShader(vertex_shader);
//
	//fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	//glCompileShader(fragment_shader);
	//program = glCreateProgram();
//
	//glAttachShader(program, vertex_shader);
	//glAttachShader(program, fragment_shader);
	//glLinkProgram(program);
//
	// "uniform mat4 MVP;\n"
	// "uniform vec3 meshColour; \n"
	mvp_location = glGetUniformLocation(shadProgID, "MVP");		// program

	//uniform mat4 matModel;
	//uniform mat4 matView;
	//uniform mat4 matProjection;
	GLint matModel_UniLoc = glGetUniformLocation(shadProgID, "matModel");
	GLint matView_Uniloc = glGetUniformLocation(shadProgID, "matView");
	GLint matProj_Uniloc = glGetUniformLocation(shadProgID, "matProjection");

	//vpos_location = glGetAttribLocation(shadProgID, "vPos");	// program
	//vcol_location = glGetAttribLocation(shadProgID, "vCol");	// program

	// If it returns -1, then it didn't find it.
	meshColour_UniLoc = glGetUniformLocation(shadProgID, "meshColour");
	
	// Shader uniform variables
	::g_L1.position = glm::vec3( 5.0f, 3.0f, 0.0f );
	::g_L1.attenConst = 0.0f;		// NO attenuation
	::g_L1.attenLinear = 0.1f;		// NO attenuation
	::g_L1.attenQuad = 0.0f;		// NO attenuation
	::g_L1.color = glm::vec3(1.0f, 0.0f, 0.0f);
	::g_L2.position = glm::vec3( -5.0f, 3.0f, 0.0f );
	::g_L2.attenConst = 0.0f;		// NO attenuation
	::g_L2.attenLinear = 0.1f;		// NO attenuation
	::g_L2.attenQuad = 0.0f;		// NO attenuation
	::g_L2.color = glm::vec3(0.972f, 1.0f, 0.101f);
	::g_L3.position = glm::vec3( -5.0f, 0.0f, 5.0f );
	::g_L3.attenConst = 0.0f;		// NO attenuation
	::g_L3.attenLinear = 0.1f;		// NO attenuation
	::g_L3.attenQuad = 0.0f;		// NO attenuation
	::g_L3.color = glm::vec3(0.0f, 0.0f, 1.0f);

	//glEnableVertexAttribArray(vcol_location);	// vCol
	//glVertexAttribPointer(vcol_location, 3,		// vCol
	//					   GL_FLOAT, GL_FALSE,
	//					   sizeof(float) * 6, 
	//					   ( void* )( sizeof(float) * 3 ));

	::g_pTheVAOManager = new cVAOManager();
	
//	"bun_zipper_res2_xyz.ply", "ssj100_xyz.ply", "building_xyz.ply"
	sModelDrawInfo bunny;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "mig29.ply", bunny, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}
	sModelDrawInfo cow;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "s21.ply", cow, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}	
	sModelDrawInfo airplane;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "ssj100.ply", airplane, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}
	sModelDrawInfo arena;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "su47.ply", arena, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}
	sModelDrawInfo shuttle;
	if (!::g_pTheVAOManager->LoadModelIntoVAO("SpaceShuttleOrbiter.ply", shuttle, shadProgID))
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}
	sModelDrawInfo terrain;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "CrappyTerrain.ply", terrain, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}
	sModelDrawInfo sphere;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "isosphere_xyz.ply", sphere, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}

	// If you want to draw lines that aren't filled, you 
	//	can change the "polygon mode" to "LINE" 
	// (Note: there IS a GL_LINES, with an "S", which ISN'T
	//  what you want here...)
	//
//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// Print out the GL version
	std::cout << glGetString( GL_VERSION ) << std::endl;

	glm::vec3 cameraEye = glm::vec3( 0.0, 0.0, -2.0f ); 
	glm::vec3 cameraTarget = glm::vec3( 0.0f, 0.0f, 0.0f ); 
	glm::vec3 upVector = glm::vec3( 0.0f, 0.1f, 0.0f );

	float theAmazingScale = 1.0f;

	glEnable( GL_DEPTH_TEST );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		
		//mat4x4 m, p, mvp;
		glm::mat4 matModel;			// was "m" (also "world")
		glm::mat4 matProjection;	// was "p"
		glm::mat4 matView;			// was "view"

		glm::mat4 mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / ( float )height;
		glViewport(0, 0, width, height);
//		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		matProjection = glm::perspective( 0.6f, ratio, 0.1f,100000.0f );		// Far plane

		matView = glm::mat4(1.0f);

		// Deal with the keyboard, etc.
		ProcessInput( cameraEye, cameraTarget, window );

		//int state = glfwGetKey(window, GLFW_KEY_D);
		//if (state == GLFW_PRESS)
		//{
		//	cameraEye.x += 0.01f;
		//}
		//state = glfwGetKey(window, GLFW_KEY_A);
		//if (state == GLFW_PRESS)
		//{
		//	cameraEye.x -= 0.01f;
		//}

		//glm::vec3 cameraEye = glm::vec3( 0.0, 0.0, -2.0f ); 
		//glm::vec3 cameraTarget = glm::vec3( 0.0f, 0.0f, 0.0f ); 
		//glm::vec3 upVector = glm::vec3( 0.0f, 1.0f, 0.0f );

		matView = glm::lookAt( cameraEye,		// position (3d space)
							   cameraTarget,	// looking at
							   upVector );		// Up vector

//		glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );

		// Set the lighting for the entire scene...
		// vec3 lightPosition: 

	
		//Light 1
		GLint lightColour1_UL = glGetUniformLocation(shadProgID, "lightColour1");
		glUniform3f(lightColour1_UL, ::g_L1.color.r, ::g_L1.color.g, ::g_L1.color.b);
		GLint LightPos1_UL = glGetUniformLocation(shadProgID, "lightPosition1");
		glUniform3f(LightPos1_UL, ::g_L1.position.x, ::g_L1.position.y, ::g_L1.position.z);
		GLint LightAttenType1_UL = glGetUniformLocation(shadProgID, "lightAttenAndType1");
		glUniform4f(LightAttenType1_UL, ::g_L1.attenConst, ::g_L1.attenLinear, ::g_L1.attenQuad, 0.0f);

		//Light 2
		GLint LightPos2_UL = glGetUniformLocation(shadProgID, "lightPosition2");
		glUniform3f(LightPos2_UL, ::g_L2.position.x, ::g_L2.position.y, ::g_L2.position.z);
		GLint lightColour2_UL = glGetUniformLocation(shadProgID, "lightColour2");
		glUniform3f(lightColour2_UL, ::g_L2.color.r, ::g_L2.color.g, ::g_L2.color.b);
		GLint LightAttenType2_UL = glGetUniformLocation(shadProgID, "lightAttenAndType2");
		glUniform4f(LightAttenType2_UL, ::g_L2.attenConst, ::g_L2.attenLinear, ::g_L2.attenQuad, 0.0f);

		//Light 3
		GLint lightColour3_UL = glGetUniformLocation(shadProgID, "lightColour3");
		glUniform3f(lightColour3_UL, ::g_L3.color.r, ::g_L3.color.g, ::g_L3.color.b);
		GLint LightPos3_UL = glGetUniformLocation(shadProgID, "lightPosition3");
		glUniform3f( LightPos3_UL, ::g_L3.position.x, ::g_L3.position.y, ::g_L3.position.z );
		GLint LightAttenType3_UL = glGetUniformLocation(shadProgID, "lightAttenAndType3");
		glUniform4f( LightAttenType3_UL, ::g_L3.attenConst, ::g_L3.attenLinear, ::g_L3.attenQuad,  0.0f );


		// move that light mesh to where the light is at, yo
		::g_vec_pMeshObjects[0]->pos = ::g_L1.position;
		::g_vec_pMeshObjects[1]->pos = ::g_L2.position;
		::g_vec_pMeshObjects[2]->pos = ::g_L3.position;


		unsigned int numberOfObjects = 
			static_cast<unsigned int>(::g_vec_pMeshObjects.size() );

		for ( unsigned int meshIndex = 0;
			  meshIndex != numberOfObjects; meshIndex++ )
		{
			cMeshObject* pCurMesh = ::g_vec_pMeshObjects[meshIndex];

			GLint AffectedByLight_UL = glGetUniformLocation(shadProgID, "AffectedByLight");
			glUniform1f(AffectedByLight_UL, pCurMesh->isAffectedByLight);

			GLint isUniformColour_UL = glGetUniformLocation(shadProgID, "bUseVertexColour");
			glUniform1f(isUniformColour_UL, pCurMesh->isUniformColour);

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


			//mat4x4_mul(mvp, p, m);
			// mvp = p * view * m; 



			// Also set the colour...
			glUniform3f( meshColour_UniLoc,			// 'meshColour' in shader
						 pCurMesh->colour.x, 
						 pCurMesh->colour.y,
						 pCurMesh->colour.z );


			GLint matModelLoc = -1;
			GLint matProjectionLoc = -1;
			GLint matViewLoc = -1;

			matModelLoc = glGetUniformLocation(shadProgID, "matModel");
			matViewLoc = glGetUniformLocation(shadProgID, "matView");
			matProjectionLoc = glGetUniformLocation(shadProgID, "matProjection");
			glUniformMatrix4fv(matModelLoc, 1, GL_FALSE, glm::value_ptr(matModel));
			glUniformMatrix4fv(matProjectionLoc, 1, GL_FALSE, glm::value_ptr(matProjection));
			glUniformMatrix4fv(matViewLoc, 1, GL_FALSE, glm::value_ptr(matView));

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
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // while (!glfwWindowShouldClose(window))


	ShutErDown();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

//void LoadTheModel(std::string fileName)
//{
//	// Open the file. 
//	// Read until we hit the word "vertex"
//	// Read until we hit the word "face"
//	// Read until we hit the word "end_header"
//
//	std::ifstream thePlyFile( fileName.c_str() );
//	if ( ! thePlyFile.is_open() )
//	{	// Something is wrong...
//		return;
//	}
//
//	std::string temp; 
//	while ( thePlyFile >> temp )
//	{
//		if ( temp == "vertex" ) 
//		{
//			break;
//		}
//	}; 
//	// read the number of vertices...
//	thePlyFile >> ::g_NumberOfVertices;
//
//	while ( thePlyFile >> temp )
//	{
//		if ( temp == "face" ) 
//		{
//			break;
//		}
//	}; 
//	// read the number of triangles...
//	thePlyFile >> ::g_NumberOfTriangles;
//
//
//	while ( thePlyFile >> temp )
//	{
//		if ( temp == "end_header" ) 
//		{
//			break;
//		}
//	}; 
//
//	// And now, we start reading vertices... Hazzah!
//	
//	// This is set up to match the ply (3d model) file. 
//	// NOT the shader. 
//	struct sVertPly
//	{
//		glm::vec3 pos;
//		glm::vec4 colour;
//	};
//
//	std::vector<sVertPly> vecTempPlyVerts;
//
//	sVertPly tempVert;
//	// Load the vertices...
//	for ( unsigned int index = 0; index != ::g_NumberOfVertices; index++ )
//	{
//		thePlyFile >> tempVert.pos.x >> tempVert.pos.y >> tempVert.pos.z;
//		
////		tempVert.pos.x *= 10.0f;
////		tempVert.pos.y *= 10.0f;
////		tempVert.pos.z *= 10.0f;
//
//
//		thePlyFile >> tempVert.colour.x >> tempVert.colour.y
//			       >> tempVert.colour.z >> tempVert.colour.w; 
//
//		// Scale the colour from 0 to 1, instead of 0 to 255
//		tempVert.colour.x /= 255.0f;
//		tempVert.colour.y /= 255.0f;
//		tempVert.colour.z /= 255.0f;
//
//		// Add too... what? 
//		vecTempPlyVerts.push_back(tempVert);
//	}
//
//	struct sTriPly
//	{
//		unsigned int vindex[3];		// the 3 indices
//		sVertPly verts[3];			// The actual vertices
//	};
//
//	std::vector<sTriPly> vecTempPlyTriangles;
//	sTriPly tempTriangle;
//	unsigned int discard = 0;
//	for ( unsigned int index = 0; index != ::g_NumberOfTriangles; index++ )
//	{
//		// 3 5622 5601 5640
//		thePlyFile >> discard						// the "3" at the start
//			       >> tempTriangle.vindex[0]
//			       >> tempTriangle.vindex[1]
//			       >> tempTriangle.vindex[2];
//
//		// Look up the vertex that matches the triangle index values.
//		tempTriangle.verts[0] = vecTempPlyVerts[ tempTriangle.vindex[0] ];
//		tempTriangle.verts[1] = vecTempPlyVerts[ tempTriangle.vindex[1] ];
//		tempTriangle.verts[2] = vecTempPlyVerts[ tempTriangle.vindex[2] ];
//
//		vecTempPlyTriangles.push_back( tempTriangle );
//	}
//	
//	// NOW, we need to put them into the vertex array buffer that 
//	//	will be passed to OpenGL. Why? 
//	// Because we called glDrawArrays(), that's why. 
//
//	::g_NumberOfVertsToDraw = ::g_NumberOfTriangles * 3;	// 3 because "triangles"
//
//	// sVert* pVertices = 0;
//	pVertices = new sVert[::g_NumberOfVertsToDraw];
//	// Allocate on the HEAP, so infinite size... 
//	// delete pVertices			/// error!
//	// delete [] pVertices		/// correct!!
//	unsigned int vertIndex = 0;
//	for ( unsigned int triIndex = 0; 
//		  triIndex != ::g_NumberOfTriangles; 
//		  triIndex++, vertIndex += 3 )
//	{
//		sTriPly& curTri = vecTempPlyTriangles[triIndex];
//
//		pVertices[ vertIndex + 0 ].x = curTri.verts[0].pos.x;
//		pVertices[ vertIndex + 0 ].y = curTri.verts[0].pos.y;
//		pVertices[ vertIndex + 0 ].z = curTri.verts[0].pos.z;
//		pVertices[ vertIndex + 0 ].r = curTri.verts[0].colour.x;
//		pVertices[ vertIndex + 0 ].g = curTri.verts[0].colour.y;
//		pVertices[ vertIndex + 0 ].b = curTri.verts[0].colour.z;
//
//		pVertices[ vertIndex + 1 ].x = curTri.verts[1].pos.x;
//		pVertices[ vertIndex + 1 ].y = curTri.verts[1].pos.y;
//		pVertices[ vertIndex + 1 ].z = curTri.verts[1].pos.z;
//		pVertices[ vertIndex + 1 ].r = curTri.verts[1].colour.x;
//		pVertices[ vertIndex + 1 ].g = curTri.verts[1].colour.y;
//		pVertices[ vertIndex + 1 ].b = curTri.verts[1].colour.z;
//
//		pVertices[ vertIndex + 2 ].x = curTri.verts[2].pos.x;
//		pVertices[ vertIndex + 2 ].y = curTri.verts[2].pos.y;
//		pVertices[ vertIndex + 2 ].z = curTri.verts[2].pos.z;
//		pVertices[ vertIndex + 2 ].r = curTri.verts[2].colour.x;
//		pVertices[ vertIndex + 2 ].g = curTri.verts[2].colour.y;
//		pVertices[ vertIndex + 2 ].b = curTri.verts[2].colour.z;
//
//	}// for ( unsigned int triIndex = 0...
//
//	return;
//}
int prevTime = 0;
bool movedThisSecond = false;
void ProcessInput( glm::vec3 &cameraEye, glm::vec3 &cameraTarget, GLFWwindow* &window )
{
	float cameraSpeed = 0.1f; 

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

	//PARTY
	int count = floor(glfwGetTime());
	if (prevTime != count){
		prevTime = count;
		movedThisSecond = false;
	}
	else {
		movedThisSecond = true;
	}
	state = glfwGetKey(window, GLFW_KEY_P);	// Down
	if (state == GLFW_PRESS) { 
		if (count % 1 == 0 && !movedThisSecond) {
			::g_L1.position = glm::vec3(((rand() % 10) - (rand() % 10)), 0.0f, ((rand() % 10) - (rand() % 10)));
			::g_L2.position = glm::vec3(((rand() % 10) - (rand() % 10)), 0.0f, ((rand() % 10) - (rand() % 10)));
			::g_L3.position = glm::vec3(((rand() % 10) - (rand() % 10)), 0.0f, ((rand() % 10) - (rand() % 10)));

			::g_L1.color = glm::vec3(rand() % 256 / 255.0f, rand() % 256 / 255.0f, rand() % 256 / 255.0f);
			::g_L2.color = glm::vec3(rand() % 256 / 255.0f, rand() % 256 / 255.0f, rand() % 256 / 255.0f);
			::g_L3.color = glm::vec3(rand() % 256 / 255.0f, rand() % 256 / 255.0f, rand() % 256 / 255.0f);

			::g_L1.attenConst = 0.0f;
			::g_L1.attenLinear = 1.0f;
			::g_L1.attenQuad = 0.0f;

			::g_L2.attenConst = 0.0f;
			::g_L2.attenLinear = 1.0f;
			::g_L2.attenQuad = 0.0f;

			::g_L3.attenConst = 0.0f;
			::g_L3.attenLinear = 1.0f;
			::g_L3.attenQuad = 0.0f;
		}
	}
	else {

	}

	// Adjust the lighting
	if ( glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS )
	{	// Decrease linear atten by 1%
		if (selectedLight == 0) {
			g_L1.attenLinear *= 0.99f;			// -1%
		}
		else if (selectedLight == 1) {
			g_L2.attenLinear *= 0.99f;			// -1%
		}
		else if (selectedLight == 2) {
			g_L3.attenLinear *= 0.99f;			// -1%
		}
	}

	if ( glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS )
	{	// Increase linear atten by 1%
		if (selectedLight == 0) {
			if (g_L1.attenLinear <= 0.0f)
			{
				g_L1.attenLinear = 0.01f;		// Make it a tiny value
			}
			else
			{
				g_L1.attenLinear *= 1.01f;		// + 1%
				if (g_L1.attenLinear >= 1.0f)
				{
					g_L1.attenLinear = 1.0f;		// Saturate to 1.0f
				}
			}
		}
		else if (selectedLight == 1) {
			if (g_L2.attenLinear <= 0.0f)
			{
				g_L2.attenLinear = 0.01f;		// Make it a tiny value
			}
			else
			{
				g_L2.attenLinear *= 1.01f;		// + 1%
				if (g_L2.attenLinear >= 1.0f)
				{
					g_L2.attenLinear = 1.0f;		// Saturate to 1.0f
				}
			}
		}
		else if (selectedLight == 2) {
			if (g_L3.attenLinear <= 0.0f)
			{
				g_L3.attenLinear = 0.01f;		// Make it a tiny value
			}
			else
			{
				g_L3.attenLinear *= 1.01f;		// + 1%
				if (g_L3.attenLinear >= 1.0f)
				{
					g_L3.attenLinear = 1.0f;		// Saturate to 1.0f
				}
			}
		}
	}
	if ( glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS )
	{	// Decrease quadratic atten by 1%
		if (selectedLight == 0) {
			g_L1.attenQuad *= 0.99f;			// +1%
		}
		else if (selectedLight == 1) {
			g_L2.attenQuad *= 0.99f;			// +1%

		}
		else if (selectedLight == 2) {
			g_L2.attenQuad *= 0.99f;			// +1%

		}
	}

	if ( glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS )
	{	// Increase quadratic atten by 1%
		if (selectedLight == 0) {

			if (g_L1.attenQuad <= 0.0f)
			{
				g_L1.attenQuad = 0.01f;		// Make it a tiny value
			}
			else
			{
				g_L1.attenQuad *= 1.01f;		// + 1%
				if (g_L1.attenQuad >= 1.0f)
				{
					g_L1.attenQuad = 1.0f;		// Saturate to 1.0f
				}
			}
		}
		else if (selectedLight == 1) {

			if (g_L2.attenQuad <= 0.0f)
			{
				g_L2.attenQuad = 0.01f;		// Make it a tiny value
			}
			else
			{
				g_L2.attenQuad *= 1.01f;		// + 1%
				if (g_L2.attenQuad >= 1.0f)
				{
					g_L2.attenQuad = 1.0f;		// Saturate to 1.0f
				}
			}
		}
		else if (selectedLight == 2) {

			if (g_L3.attenQuad <= 0.0f)
			{
				g_L3.attenQuad = 0.01f;		// Make it a tiny value
			}
			else
			{
				g_L3.attenQuad *= 1.01f;		// + 1%
				if (g_L3.attenQuad >= 1.0f)
				{
					g_L3.attenQuad = 1.0f;		// Saturate to 1.0f
				}
			}
		}
	}	

	// Also move the light around
	if ( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS )
	{
		if(selectedLight == 0)
			::g_L1.position.z += 0.1f;		
		else if(selectedLight == 1)
			::g_L2.position.z += 0.1f;
		else if (selectedLight == 2)
			::g_L3.position.z += 0.1f;
	}
	if ( glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS )
	{
		if (selectedLight == 0)
			::g_L1.position.z -= 0.1f;
		else if (selectedLight == 1)
			::g_L2.position.z -= 0.1f;
		else if (selectedLight == 2)
			::g_L3.position.z -= 0.1f;
	}		
	if ( glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS )
	{
		if (selectedLight == 0)
			::g_L1.position.x -= 0.1f;
		else if (selectedLight == 1)
			::g_L2.position.x -= 0.1f;
		else if (selectedLight == 2)
			::g_L3.position.x -= 0.1f;
	}	
	if ( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS )
	{
		if (selectedLight == 0)
			::g_L1.position.x += 0.1f;
		else if (selectedLight == 1)
			::g_L2.position.x += 0.1f;
		else if (selectedLight == 2)
			::g_L3.position.x += 0.1f;
	}		
	if ( glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS )
	{
		if (selectedLight == 0)
			::g_L1.position.y += 0.1f;
		else if (selectedLight == 1)
			::g_L2.position.y += 0.1f;
		else if (selectedLight == 2)
			::g_L3.position.y += 0.1f;
	}		
	if ( glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS )
	{
		if (selectedLight == 0)
			::g_L1.position.y -= 0.1f;
		else if (selectedLight == 1)
			::g_L2.position.y -= 0.1f;
		else if (selectedLight == 2)
			::g_L3.position.y -= 0.1f;
	}		


	//std::cout << "Camera (xyz): "  
	//		<<cameraEye.x << ", " 
	//		<< cameraEye.y << ", "
	//		<< cameraEye.z << std::endl;
	return;
}


//std::vector< cMeshObject* > g_vec_pMeshObjects;
void LoadObjectsIntoScene(void)
{

	/**
	std::ifstream inputFile;
	inputFile.open("config.txt");
	if (inputFile.is_open()) {
		while (inputFile.good())
		{
			std::string name;
			glm::vec3 position;
			glm::vec4 colour;
			glm::vec3 orientation;
			float scale;
			bool wireframe;

			inputFile >> name;
			inputFile >> position.x >> position.y >> position.z;
			inputFile >> colour.r >> colour.g >> colour.b >> colour.a;
			inputFile >> orientation.x >> orientation.y >> orientation.z;
			inputFile >> wireframe;
			inputFile >> scale;

			::g_vec_pMeshObjects.push_back(new cMeshObject(name, position, colour, orientation, scale, wireframe));
		}
	}*/

	{// Add an object into the "scene"
		::g_pTheLightMesh = new cMeshObject(); 

		::g_pTheLightMesh->meshName = "isosphere_xyz.ply";

		::g_pTheLightMesh->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_pTheLightMesh->colour = glm::vec4(244.0f / 255.0f, 51.0f / 255.0f, 255.0f / 255.0f, 1.0f);
		::g_pTheLightMesh->scale = 0.1f;
		::g_pTheLightMesh->isWireframe = false;
		::g_pTheLightMesh->isAffectedByLight = false;
		::g_pTheLightMesh->isUniformColour = true;

		::g_vec_pMeshObjects.push_back( ::g_pTheLightMesh );
	}	
	{// Add an object into the "scene"
		::g_pTheLightMesh = new cMeshObject(); 

		::g_pTheLightMesh->meshName = "isosphere_xyz.ply";

		::g_pTheLightMesh->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_pTheLightMesh->colour = glm::vec4(128.0f/255.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f);		// Transparency 'alpha'
		::g_pTheLightMesh->scale = 0.1f;
		::g_pTheLightMesh->isWireframe = false;
		::g_pTheLightMesh->isAffectedByLight = false;
		::g_pTheLightMesh->isUniformColour = true;

		::g_vec_pMeshObjects.push_back( ::g_pTheLightMesh );
	}	
	{// Add an object into the "scene"
		::g_pTheLightMesh = new cMeshObject(); 

		::g_pTheLightMesh->meshName = "isosphere_xyz.ply";

		::g_pTheLightMesh->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_pTheLightMesh->colour = glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);		// Transparency 'alpha'

		::g_pTheLightMesh->scale = 0.1f;
		::g_pTheLightMesh->isWireframe = false;
		::g_pTheLightMesh->isAffectedByLight = false;
		::g_pTheLightMesh->isUniformColour = true;

		::g_vec_pMeshObjects.push_back( ::g_pTheLightMesh );
	}	


	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "CrappyTerrain.ply";

		pTemp->pos = glm::vec3( 0.0f, -10.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 205.0f/255.0f, 49.0f/255.0f, 1.0f );		// Transparency 'alpha'
		pTemp->scale = 1.0f;
		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;

		::g_vec_pMeshObjects.push_back( pTemp );
	}	
	
	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "s21.ply";

		pTemp->pos = glm::vec3( 1.0f, 0.2f, 0.0f );
		pTemp->colour = glm::vec4( 243.0f/255.0f,		
								     9.0f/255.0f,
								    25.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "s21.ply";

		pTemp->orientation = glm::vec3(0.0f, 0.2f, 0.0f);
		pTemp->pos = glm::vec3( 2.0f, 1.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f,	
								   205.0f/255.0f,
								   248.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		pTemp->isWireframe = true;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "mig29.ply";

		pTemp->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "ssj100.ply";

		// 2 * PI   
		// 1 PI = 180
		// 0.5 = 90 
		// 0.25 = 45


//		pTemp->orientation.x = 3.14159f * 0.25f;		// YOLO, right? 
		pTemp->orientation.y = glm::pi<float>() * 0.5f;		// YOLO, right? 

		pTemp->orientation = glm::vec3(0.0f,6.2f, 0.0f);
		pTemp->pos = glm::vec3( -1.1f, 0.3f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 
									49.0f/255.0f,
								   205.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 1.5f;
		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "SpaceShuttleOrbiter.ply";
		pTemp->orientation = glm::vec3(-1.5f, 2.8f, 0.0f);
		pTemp->pos = glm::vec3( -2.0f, 1.0f, 0.0f );
		pTemp->colour = glm::vec4( 244.0f/255.0f,  
									223.0f/255.0f,
								    33.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		// Largest "extent" in this model
		// is 40.2828 
		
		pTemp->scale = 0.001f;

		// Now my model 1.0 unit in size (-1 to 1)

		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject();

		pTemp->meshName = "s21.ply";

		pTemp->pos = glm::vec3(1.0f, 0.2f, 2.5f);
		pTemp->colour = glm::vec4(243.0f / 255.0f,
			9.0f / 255.0f,
			25.0f / 255.0f,
			1.0f);		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back(pTemp);
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject();

		pTemp->meshName = "s21.ply";

		pTemp->orientation = glm::vec3(0.0f, 0.2f, 0.0f);
		pTemp->pos = glm::vec3(2.0f, 1.0f, 2.5f);
		pTemp->colour = glm::vec4(142.0f / 255.0f,
			205.0f / 255.0f,
			248.0f / 255.0f,
			1.0f);		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		pTemp->isWireframe = true;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back(pTemp);
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject();

		pTemp->meshName = "mig29.ply";

		pTemp->pos = glm::vec3(0.0f, 0.0f,2.5f);
		pTemp->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back(pTemp);
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject();

		pTemp->meshName = "ssj100.ply";

		// 2 * PI   
		// 1 PI = 180
		// 0.5 = 90 
		// 0.25 = 45


		//		pTemp->orientation.x = 3.14159f * 0.25f;		// YOLO, right? 
		pTemp->orientation.y = glm::pi<float>() * 0.5f;		// YOLO, right? 

		pTemp->orientation = glm::vec3(0.0f, 6.2f, 0.0f);
		pTemp->pos = glm::vec3(-1.1f, 0.3f, 2.5f);
		pTemp->colour = glm::vec4(142.0f / 255.0f,
			49.0f / 255.0f,
			205.0f / 255.0f,
			1.0f);		// Transparency 'alpha'
		pTemp->scale = 1.5f;
		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back(pTemp);
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject();

		pTemp->meshName = "SpaceShuttleOrbiter.ply";
		pTemp->orientation = glm::vec3(-1.5f, 2.8f, 0.0f);
		pTemp->pos = glm::vec3(-2.0f, 1.0f, 2.5f);
		pTemp->colour = glm::vec4(244.0f / 255.0f,
			223.0f / 255.0f,
			33.0f / 255.0f,
			1.0f);		// Transparency 'alpha'
						// Largest "extent" in this model
						// is 40.2828 

		pTemp->scale = 0.001f;

		// Now my model 1.0 unit in size (-1 to 1)

		pTemp->isWireframe = false;
		pTemp->isUniformColour = true;


		::g_vec_pMeshObjects.push_back(pTemp);
	}
	return;
}

void ShutErDown(void)
{
	delete ::g_pTheShaderManager;

	return;
}
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


	//// NOTE: OpenGL error checks have been omitted for brevity
	//glGenBuffers(1, &vertex_buffer);
	//glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	//// sVert vertices[3]
	//glBufferData( GL_ARRAY_BUFFER, 
	//			  sizeof(sVert) * ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
	//			  pVertices,			//vertices, 
	//			  GL_STATIC_DRAW);

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

	//vpos_location = glGetAttribLocation(shadProgID, "vPos");	// program
	//vcol_location = glGetAttribLocation(shadProgID, "vCol");	// program

	// If it returns -1, then it didn't find it.
	meshColour_UniLoc = glGetUniformLocation(3, "meshColour");
//struct sVert
//{
//	float x, y, z;		// added "z"
//	float r, g, b;
//};

	//glEnableVertexAttribArray(vpos_location);	// vPos
	//glVertexAttribPointer(vpos_location, 3,		// vPos
	//					   GL_FLOAT, GL_FALSE,
	//					   sizeof(float) * 6, 
	//					   ( void* )0);

	//glEnableVertexAttribArray(vcol_location);	// vCol
	//glVertexAttribPointer(vcol_location, 3,		// vCol
	//					   GL_FLOAT, GL_FALSE,
	//					   sizeof(float) * 6, 
	//					   ( void* )( sizeof(float) * 3 ));


	::g_pTheVAOManager = new cVAOManager();
	
//	"bun_zipper_res2_xyz.ply", "ssj100_xyz.ply", "building_xyz.ply"
	sModelDrawInfo bunny;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "bun_zipper_res2_xyz.ply", bunny, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}
	sModelDrawInfo cow;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "cow_xyz.ply", cow, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}	
	sModelDrawInfo airplane;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "ssj100_xyz.ply", airplane, shadProgID ) )
	{
		std::cout << "Error: Problem loading model into VAO" << std::endl;
		// We'll keep going as we might be able to load other models?
	}
	sModelDrawInfo arena;
	if ( ! ::g_pTheVAOManager->LoadModelIntoVAO( "free_arena_ASCII_xyz.ply", arena, shadProgID ) )
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

		
		// The matModel stuff WAS here. Moved below...

		//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
//		p = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f );

		matProjection 
	      = glm::perspective( 0.6f, 
							  ratio, 
							  0.1f,				// Near plane	
							  100000.0f );		// Far plane

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

		unsigned int numberOfObjects = 
			static_cast<unsigned int>(::g_vec_pMeshObjects.size() );

		for ( unsigned int meshIndex = 0;
			  meshIndex != numberOfObjects; meshIndex++ )
		{
			cMeshObject* pCurMesh = ::g_vec_pMeshObjects[meshIndex];


			//mat4x4_identity(m);
			matModel = glm::mat4(1.0f);		// because "math"

			//mat4x4_rotate_Z(m, m, );
			glm::mat4 rotateZ = glm::rotate( glm::mat4(1.0f), 
											 0.0f, // (float) glfwGetTime(), 
											 glm::vec3( 0.0f, 0.0, 1.0f ) );
			matModel = matModel * rotateZ;

			// Place the object in the world at 'this' location
			glm::mat4 matTranslation 
				= glm::translate( glm::mat4(1.0f), 
			                      glm::vec3( pCurMesh->pos.x,
								             pCurMesh->pos.y, 
								             pCurMesh->pos.z ) );
			matModel = matModel * matTranslation;
		
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
			mvp = matProjection * matView * matModel; 


			// Also set the colour...
			glUniform3f( meshColour_UniLoc,			// 'meshColour' in shader
						 pCurMesh->colour.x, 
						 pCurMesh->colour.y,
						 pCurMesh->colour.z );


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

//			glUseProgram(shadProgID);
//			::g_pTheShaderManager->useShaderProgram("simpleshader");
//			::g_pTheShaderManager->useShaderProgram( shadProgID );

	//		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, ( const GLfloat* )mvp);
			glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));


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

	std::cout << "Camera (xyz): "  
			<<cameraEye.x << ", " 
			<< cameraEye.y << ", "
			<< cameraEye.z << std::endl;
	return;
}


//std::vector< cMeshObject* > g_vec_pMeshObjects;
void LoadObjectsIntoScene(void)
{
	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "CrappyTerrain.ply";

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

		pTemp->meshName = "cow_xyz.ply";

		pTemp->pos = glm::vec3( 1.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 
								   205.0f/255.0f,
									49.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 0.05f;
		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "cow_xyz.ply";

		pTemp->pos = glm::vec3( 2.0f, 1.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 
								   205.0f/255.0f,
									49.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 0.1f;
		pTemp->isWireframe = true;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "bun_zipper_res2_xyz.ply";

		pTemp->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 205.0f/255.0f,
								   142.0f/255.0f, 
									49.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		pTemp->scale = 2.0f;
		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}

	{// Add an object into the "scene"
		cMeshObject* pTemp = new cMeshObject(); 

		pTemp->meshName = "ssj100_xyz.ply";

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

		pTemp->meshName = "free_arena_ASCII_xyz.ply";

		pTemp->pos = glm::vec3( 0.0f, 0.0f, 0.0f );
		pTemp->colour = glm::vec4( 142.0f/255.0f, 
									49.0f/255.0f,
								   205.0f/255.0f,
									 1.0f );		// Transparency 'alpha'
		// Largest "extent" in this model
		// is 40.2828 
		
		pTemp->scale = 1.0f/40.2828f;

		// Now my model 1.0 unit in size (-1 to 1)

		pTemp->isWireframe = false;

		::g_vec_pMeshObjects.push_back( pTemp );
	}



	// std::vector< cMeshObject* > ;

	return;
}

void ShutErDown(void)
{
	// 
	delete ::g_pTheShaderManager;

	return;
}


	////
	//	for ( std::vector< cMeshObject* >::iterator itMesh;
	//		  itMesh != ::g_vec_pMeshObjects.end();
	//		  itMesh++ )
	//	{
	//		cMeshObject* pCurMesh = *itMesh;





// Old sexy shaders
//static const char* vertex_shader_text =
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"		// float r, g, b;
//"attribute vec3 vPos;\n"		// float x, y, z;
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"   vec3 newVertex = vPos;				\n"
//"\n"
//"   newVertex.x = newVertex.x / 100000.0f; \n"
//"	newVertex.y = newVertex.y / 100000.0f; \n"
//"	newVertex.z = newVertex.z / 100000.0f; \n"
//"\n"
//"    gl_Position = MVP * vec4(newVertex, 1.0);\n"
//"    color = vCol;\n"
//"}\n";

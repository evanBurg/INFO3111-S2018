#include "globalStuff.h"
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "cMeshObject.h"
#include "cShaderManager.h"
#include "cVAOManager.h"

void DrawObject( cMeshObject* pCurMesh, 
				 cShaderManager::cShaderProgram* pShaderProg,	// To get at uniforms
				 cVAOManager* pVAOManager,
				 glm::mat4 matParentModel )
{
	// Is this visible
	if ( ! pCurMesh->bIsVisible )
	{
		// Skip it.
		return;
	}

	//mat4x4_identity(m);
	//matModel = glm::mat4(1.0f);		// because "math"

	glm::mat4 matModel = matParentModel;		// because "math"
			


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


	// Are we taking colour from the model file (vertex values)
	//	or as a single colour (the 'colour' value in the mesh object)
	//	passed to the vertex shader
	switch ( pCurMesh->colourSource )
	{
		case cMeshObject::USE_VERTEX_COLOURS:
			glUniform1f( pShaderProg->getUniformID_From_Name("bUseVertexColour"), 
						 GL_TRUE );
//			glUniform1f( bUseVertexColour_UniLoc, GL_TRUE );
			break;

		case cMeshObject::USE_OBJECT_COLOUR:
			glUniform1f( pShaderProg->getUniformID_From_Name("bUseVertexColour"), GL_FALSE );
			//glUniform1f( bUseVertexColour_UniLoc, GL_FALSE );
			glUniform4f( pShaderProg->getUniformID_From_Name("meshColourRGBA"),			
							pCurMesh->colour.x, 
							pCurMesh->colour.y,
							pCurMesh->colour.z,
							pCurMesh->colour.a );
			//glUniform4f( meshColourRGBA_UniLoc,			
			//				pCurMesh->colour.x, 
			//				pCurMesh->colour.y,
			//				pCurMesh->colour.z,
			//				pCurMesh->colour.a );
			break;

		default:	
			// This shouldn't happen, so set it to "hot pink" to show error
			glUniform1f( pShaderProg->getUniformID_From_Name("bUseVertexColour"), GL_FALSE );
			glUniform4f( pShaderProg->getUniformID_From_Name("meshColourRGBA"),	255.0f, 105.0f/255.0f, 180.0f/255.0f, 1.0f );
			//glUniform1f( bUseVertexColour_UniLoc, GL_FALSE );
			//glUniform4f( meshColourRGBA_UniLoc,	255.0f, 105.0f/255.0f, 180.0f/255.0f, 1.0f );
			break;

	}//switch ( pCurMesh->colourSource )

	// Alpha (transparency) taken from vertex values or mesh 'colour' value?
	glUniform1f( pShaderProg->getUniformID_From_Name("bUse_vColourRGBA_AlphaValue"),
					(pCurMesh->bUseColourAlphaValue ? (float)GL_TRUE : (float)GL_FALSE) );
	//glUniform1f( bUse_vColourRGBA_AlphaValue_UniLoc,
	//				(pCurMesh->bUseColourAlphaValue ? (float)GL_TRUE : (float)GL_FALSE) );

	// Use vertex (model) colours or overall (mesh 'colour') value for diffuse


	glUniform1f( pShaderProg->getUniformID_From_Name("bDontLightObject"),
					(pCurMesh->bDontLightObject ? (float)GL_TRUE : (float)GL_FALSE) );
	//glUniform1f( bDontLightObject_UniLoc,
	//				(pCurMesh->bDontLightObject ? (float)GL_TRUE : (float)GL_FALSE) );


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


	glUniformMatrix4fv( pShaderProg->getUniformID_From_Name("matModel"),	//matModel_UniLoc, 
						1, 
						GL_FALSE, 
						glm::value_ptr(matModel));	

	//glUniformMatrix4fv( pShaderProg->getUniformID_From_Name("matView"),		//matView_Uniloc, 
	//					1, 
	//					GL_FALSE, 
	//					glm::value_ptr(matView));	

	//glUniformMatrix4fv( pShaderProg->getUniformID_From_Name("matProj"),		//matProj_Uniloc, 
	//					1, 
	//					GL_FALSE, 
	//					glm::value_ptr(matProjection));	


	//		glDrawArrays(GL_TRIANGLES, 0, 3);
	//		glDrawArrays(GL_TRIANGLES, 0, ::g_NumberOfVertsToDraw);

	// Figure out what model we are loading
	sModelDrawInfo modelInfo;
	if ( pVAOManager->FindDrawInfoByModelName( pCurMesh->meshName, modelInfo ) )
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


	return;
}

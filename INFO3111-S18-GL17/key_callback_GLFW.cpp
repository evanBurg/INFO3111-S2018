#include "globalStuff.h"
#include "SaveAndLoadObjects.h"

// This one is connected to the regular "keyboard" handler in Winders.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

		if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
			::g_lookAtObject = !::g_lookAtObject;
		if (mods == GLFW_MOD_SHIFT) {
			if (!::g_lookAtObject) {
				if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS && ::g_SelectedLightID - 1 > -1)
					::g_SelectedLightID -= 1;
				if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS && ::g_SelectedLightID + 1 < NUMLIGHTS)
					::g_SelectedLightID += 1;
			}
			else {
				if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS && ::g_SelectedObjectID - 1 > -1)
					::g_SelectedObjectID -= 1;
				if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS && ::g_SelectedObjectID + 1 < ::g_vec_pMeshObjects.size() - 1)
					::g_SelectedObjectID += 1;
			}
		}
	
	}//if ( mods == GLFW_MOD_SHIFT )

	if (mods == GLFW_MOD_CONTROL) {
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
			save_to_file();
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
			read_from_file();
	}

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

#include "CallBack.h"
#include "Camera.h"
#include "AppAttribs.h"
#include "Ray.h"

static AppAttribs &appAttribs = AppAttribs::GetAppAttribs();
Camera &camera = Camera::GetCamera();

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (appAttribs.GetEscaped())
			return;

		appAttribs.SetEscaped(true);
		appAttribs.InvertMenuModeStatus();
		glfwSetInputMode(window, GLFW_CURSOR, appAttribs.GetMenuMode() ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		appAttribs.SetFirstMouse(true);


	}
	else
		appAttribs.SetEscaped(false);

	double deltaTime = appAttribs.GetDeltaTime();


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.DispatchKeyboardEvent(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.DispatchKeyboardEvent(BACK, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.DispatchKeyboardEvent(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.DispatchKeyboardEvent(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.DispatchKeyboardEvent(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.DispatchKeyboardEvent(DOWN, deltaTime);
}

void CallBackStates::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	appAttribs.SetFrameBuffSizes(width, height);
	glViewport(0, 0, width, height);
}


void CallBackStates::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (appAttribs.GetMenuMode())
		return;

	if (appAttribs.GetFirstMouse())
	{
		appAttribs.SetPrevMousePos(xpos, ypos);
		appAttribs.SetFirstMouse(false);
	}

	float xoffset = xpos - appAttribs.GetPrevMouseX();
	float yoffset = -ypos + appAttribs.GetPrevMouseY();
	appAttribs.SetPrevMousePos(xpos, ypos);

	camera.DispatchMouseMoveEvent(xoffset, yoffset);
}

void CallBackStates::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.DispatchMouseScrollEvent(yoffset);
}

void CallBackStates::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (appAttribs.GetMenuMode() == false)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			Ray r(camera.GetPos());
			r.Cast(camera.GetOrientation(), 2.f);
		}
	}
}

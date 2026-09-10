#include "CallBack.h"
#include "Camera.h"
#include "AppAttribs.h"
#include "Ray.h"
#include "World.h"

static AppAttribs &app_attribs = AppAttribs::get_app_attribs();
Camera &camera = Camera::get_camera();

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (app_attribs.get_escaped())
			return;

		app_attribs.set_escaped(true);
		app_attribs.invert_menu_mode_status();
		glfwSetInputMode(window, GLFW_CURSOR, app_attribs.get_menu_mode() ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		app_attribs.set_first_mouse(true);


	}
	else
		app_attribs.set_escaped(false);

	double delta_time = app_attribs.get_delta_time();


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.dispatch_keyboard_event(NORTH, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.dispatch_keyboard_event(SOUTH, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.dispatch_keyboard_event(WEST, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.dispatch_keyboard_event(EAST, delta_time);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.dispatch_keyboard_event(UP, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.dispatch_keyboard_event(DOWN, delta_time);
}

void CallBackStates::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	app_attribs.set_frame_buff_sizes(width, height);
	glViewport(0, 0, width, height);
}


void CallBackStates::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (app_attribs.get_menu_mode())
		return;

	if (app_attribs.get_first_mouse())
	{
		app_attribs.set_prev_mouse_pos(xpos, ypos);
		app_attribs.set_first_mouse(false);
	}

	float xoffset = xpos - app_attribs.get_prev_mouse_x();
	float yoffset = -ypos + app_attribs.get_prev_mouse_y();
	app_attribs.set_prev_mouse_pos(xpos, ypos);

	camera.dispatch_mouse_move_event(xoffset, yoffset);
}

void CallBackStates::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.dispatch_mouse_scroll_event(yoffset);
}

void CallBackStates::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (app_attribs.get_menu_mode() == false)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			World *world = static_cast<World *>(glfwGetWindowUserPointer(window));
			if (!world)
				return;
			Ray r(camera.get_pos());
			r.cast(*world, camera.get_orientation(), 2.f);
		}
	}
}

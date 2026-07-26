#pragma once

#include <glfw/glfw3.h>
#include "Camera.h"

class CallBackStates {
public:
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

void processInput(GLFWwindow* window);

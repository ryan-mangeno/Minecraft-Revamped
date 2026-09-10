#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

class CallBackStates {
public:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

void process_input(GLFWwindow* window);

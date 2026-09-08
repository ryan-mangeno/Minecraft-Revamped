#include "Debug.h"
#include "Camera.h"
Camera& cam = Camera::get_camera();

void clear_gl()
{
	GLenum err;
	while (err = glGetError());
}

bool get_error(const char* function, const char* file, int line)
{
	GLenum error = glGetError();

	if (error != GL_NO_ERROR)
	{
		std::cout << "[OpenGL Error] : " << error << " line: " << line << ", Function: " << file << " from: " << function << std::endl;
		return false;
	}

	return true;
}


Gui::Gui(GLFWwindow* window) : m_window(window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = &ImGui::GetIO();
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

}

Gui::~Gui()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::handle_gui()
{

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always); // Position the window

	// Create a named window
	ImGui::Begin("-~+");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_io->Framerate, m_io->Framerate);
	glm::vec3 pos = cam.get_pos();
	ImGui::Text("x %.3f\t y %.3f\t z %.3f", pos.x, pos.y, pos.z);
	ImGui::End();

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


}

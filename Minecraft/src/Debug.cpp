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

void Gui::handle_gui(const std::vector<DebugTexture> &debug_textures)
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

	ImGui::SetNextWindowPos(ImVec2(10, 85), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(430, 460), ImGuiCond_FirstUseEver);
	ImGui::Begin("Framebuffer Debug");
	if (ImGui::BeginTabBar("FramebufferTextures")) {
		for (const DebugTexture &debug_texture : debug_textures) {
			if (debug_texture.texture_id == 0 || debug_texture.width <= 0 ||
				debug_texture.height <= 0)
				continue;

			if (ImGui::BeginTabItem(debug_texture.name)) {
				ImGui::Text("Texture %u | %d x %d", debug_texture.texture_id,
							debug_texture.width, debug_texture.height);

				ImVec2 available = ImGui::GetContentRegionAvail();
				float preview_width = available.x;
				float preview_height = preview_width *
					static_cast<float>(debug_texture.height) /
					static_cast<float>(debug_texture.width);
				if (preview_height > available.y && preview_height > 0.0f) {
					const float scale = available.y / preview_height;
					preview_width *= scale;
					preview_height *= scale;
				}

				const ImVec2 uv0 = debug_texture.flip_vertical ? ImVec2(0.0f, 1.0f)
																 : ImVec2(0.0f, 0.0f);
				const ImVec2 uv1 = debug_texture.flip_vertical ? ImVec2(1.0f, 0.0f)
																 : ImVec2(1.0f, 1.0f);
				ImGui::Image(
					reinterpret_cast<ImTextureID>(
						static_cast<intptr_t>(debug_texture.texture_id)),
					ImVec2(preview_width, preview_height), uv0, uv1);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


}

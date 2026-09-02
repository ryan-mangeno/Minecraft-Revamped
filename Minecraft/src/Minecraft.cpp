#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include "AppAttribs.h"
#include "CallBack.h"
#include "Camera.h"
#include "Debug.h"
#include "Log.h"
#include "Minecraft.h"
#include "Shader.h"
#include "Texture.h"
#include "World.h"
#include "constants.h"

GLFWwindow *window = nullptr;

void Minecraft::Run() {

  if (!initGL()) {
    MC_FATAL("Initlization Failed!");
    return;
  }

  Shader::InitShaders();
  Texture::InitTextures();

  Shader *main_shader = Shader::getShader("main_shader");
  Shader *model_shader = Shader::getShader("model_shader");

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Gui gui(window);
  World &world = World::GetWorld();
  Camera &camera = Camera::GetCamera();
  AppAttribs &appAttribs = AppAttribs::GetAppAttribs();

  float prevTime = 0.0f;

  while (!glfwWindowShouldClose(window)) {

    main_shader->Bind();
    float curTime = glfwGetTime();
    appAttribs.SetDeltaTime(curTime - prevTime);
    prevTime = curTime;

    processInput(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gui.HandleGui();

    glm::mat4 view = camera.CalcViewMatrix();

    main_shader->SetUniformMat4f("view", view);
    main_shader->SetUniformMat4f("projection", camera.GetProjMat());
    model_shader->Bind();
    model_shader->SetUniformMat4f("view", view);
    model_shader->SetUniformMat4f("projection", camera.GetProjMat());

    world.Update(camera.GetPos(), main_shader);
    world.Render(main_shader, model_shader);
    camera.OnUpdate(appAttribs.GetDeltaTime());

    glfwPollEvents();
    glfwSwapBuffers(window);
  }
}

Minecraft::~Minecraft() {
  Shader::DeleteShaders();
  Texture::DeleteTextures();
  glfwTerminate();
  glfwDestroyWindow(window);
}

bool Minecraft::initGL() {

  bool success = true;

  // Initialize GLFW
  if (!glfwInit())
    success = false;

  else {

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    window =
        glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minecraft", NULL, NULL);

    if (window == NULL) {
      MC_ERROR("Failed to create GLFW window");
      glfwTerminate();
      success = false;
      return success;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      MC_ERROR("Failed to initialize GLAD");
      success = false;
      return success;
    }

    MC_DEBUG("OpenGL: {}",
             reinterpret_cast<const char *>(glGetString(GL_VERSION)));

    MC_DEBUG("GLSL: {}", reinterpret_cast<const char *>(
                             glGetString(GL_SHADING_LANGUAGE_VERSION)));

    // Configure viewport and rendering
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window,
                                   CallBackStates::framebufferSizeCallback);
    glfwSetCursorPosCallback(window, CallBackStates::mouse_callback);
    glfwSetScrollCallback(window, CallBackStates::scroll_callback);
    glfwSetMouseButtonCallback(window, CallBackStates::mouse_button_callback);

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
  }

  return success;
}

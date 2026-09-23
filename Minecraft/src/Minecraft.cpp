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
#include "Hdr.h"
#include "Log.h"
#include "Minecraft.h"
#include "Shader.h"
#include "Texture.h"
#include "World.h"
#include "constants.h"

GLFWwindow *window = nullptr;

void Minecraft::run() {

  if (!init_gl()) {
    MC_FATAL("Initlization Failed!");
    return;
  }

  Shader::init_shaders();
  Texture::init_textures();

  Shader *main_shader = Shader::get_shader("main_shader");
  Shader *model_shader = Shader::get_shader("model_shader");

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Gui gui(window);
  World world;
  HDR hdr;
  int framebuffer_width = 0;
  int framebuffer_height = 0;
  glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
  if (!hdr.init(framebuffer_width, framebuffer_height)) {
    MC_ERROR("HDR framebuffer creation failed!");
    return;
  }

  glfwSetWindowUserPointer(window, &world);
  Camera &camera = Camera::get_camera();
  AppAttribs &app_attribs = AppAttribs::get_app_attribs();

  float prev_time = 0.0f;

  while (!glfwWindowShouldClose(window)) {

    main_shader->bind();
    float cur_time = glfwGetTime();
    app_attribs.set_delta_time(cur_time - prev_time);
    prev_time = cur_time;

    process_input(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gui.handle_gui();

    glm::mat4 view = camera.calc_view_matrix();

    main_shader->set_uniform_mat4f("view", view);
    main_shader->set_uniform_mat4f("projection", camera.get_proj_mat());
    model_shader->bind();
    model_shader->set_uniform_mat4f("view", view);
    model_shader->set_uniform_mat4f("projection", camera.get_proj_mat());

    float dt = app_attribs.get_delta_time();
    world.update(camera.get_pos(), main_shader, dt);
    camera.on_update(dt, world);

    world.render(main_shader, model_shader);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwSetWindowUserPointer(window, nullptr);
}

Minecraft::~Minecraft() {
  Shader::delete_shaders();
  Texture::delete_textures();
  glfwTerminate();
  glfwDestroyWindow(window);
}

bool Minecraft::init_gl() {

  bool success = true;

  // Initialize GLFW
  if (!glfwInit())
    success = false;

  else {

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    window =
        glfwCreateWindow(screen_width, screen_height, "Minecraft", NULL, NULL);

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
    glViewport(0, 0, screen_width, screen_height);
    glfwSetFramebufferSizeCallback(window,
                                   CallBackStates::framebuffer_size_callback);
    glfwSetCursorPosCallback(window, CallBackStates::mouse_callback);
    glfwSetScrollCallback(window, CallBackStates::scroll_callback);
    glfwSetMouseButtonCallback(window, CallBackStates::mouse_button_callback);

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
  }

  return success;
}

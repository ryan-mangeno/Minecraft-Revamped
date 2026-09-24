#include "Shader.h"
#include "Log.h"
#include "render_defines.h"
#include "util.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp> // for value_ptr

std::unordered_map<std::string, Shader *> Shader::m_shader_location_cache = {};

// Constructor that build the Shader Program from 2 different shaders
Shader::Shader(const std::string &filepath) : m_uniform_location_cache() {

  // Read vertexFile and fragmentFile
  ShaderProgramSource shader_code = get_file_contents(filepath);

  // Convert the shader source strings into character arrays
  const char *vertex_source = shader_code.vertex_source.c_str();
  const char *fragment_source = shader_code.fragment_source.c_str();

  MC_DEBUG("Vertex Shader:\n {}", vertex_source);
  MC_DEBUG("Fragment Shader:\n {}", fragment_source);

  // Create Vertex Shader Object and get its reference
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  // Attach Vertex Shader source to the Vertex Shader Object
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  // Compile the Vertex Shader into machine code
  glCompileShader(vertex_shader);
  compile_errors(vertex_shader, "VERTEX");

  // Create Fragment Shader Object and get its reference
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  // Attach Fragment Shader source to the Fragment Shader Object
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  // Compile the Vertex Shader into machine code
  glCompileShader(fragment_shader);
  compile_errors(fragment_shader, "FRAGMENT");

  // Create Shader Program Object and get its reference
  m_id = glCreateProgram();

  // Attach the Vertex and Fragment Shaders to the Shader Program
  glAttachShader(m_id, vertex_shader);
  glAttachShader(m_id, fragment_shader);

  glLinkProgram(m_id);
  compile_errors(m_id, "PROGRAM");

  glValidateProgram(m_id);

  // Delete the now useless Vertex and Fragment Shader objects
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

// Reads a text file and outputs a string with everything in the text file
ShaderProgramSource get_file_contents(const std::string &filename) {
  std::ifstream in(filename);

  if (!in.is_open()) {
    throw std::runtime_error("Failed to open shader file: " + filename);
  }

  enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

  std::stringstream ss[2];

  ShaderType type = ShaderType::NONE;

  std::string line;

  while (getline(in, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) {
        type = ShaderType::FRAGMENT;
      }
    } else if (type != ShaderType::NONE) {
      ss[(int)type] << line << '\n';
    }
  }

  return {ss[(int)ShaderType::VERTEX].str(),
          ss[(int)ShaderType::FRAGMENT].str()};
}

void Shader::set_uniform_mat4f(const std::string &uni_name,
                               const glm::mat4 &matrix) {
  // obv id, then num of matrix's which is 1, then whether or not we have a row
  // major matrix or column to transpose, since its col we dont
  glUniformMatrix4fv(get_uniform(uni_name), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}

void Shader::set_uniform_vec3_array(const std::string &uni_name,
                                    const std::vector<glm::vec3> &values) {
  if (values.empty())
    return;

  glUniform3fv(get_uniform(uni_name), static_cast<GLsizei>(values.size()),
               glm::value_ptr(values.front()));
}

void Shader::set_uniform_vec3_array(const std::string &uni_name,
                                    const float *base_ptr, size_t count) {
  if (!base_ptr)
    return;

  glUniform3fv(get_uniform(uni_name), static_cast<GLsizei>(count), base_ptr);
}

void Shader::init_shaders() {
  const auto shader_path = get_resource_path() / "shaders";

  Shader *main_shader = new Shader((shader_path / "shader.glsl").string());
  Shader *model_shader = new Shader((shader_path / "model.glsl").string());
  Shader *depth_shader =
      new Shader((shader_path / "depth_shader.glsl").string());
  Shader *hdr_shader = new Shader((shader_path / "hdr.glsl").string());

  m_shader_location_cache["main_shader"] = main_shader;
  m_shader_location_cache["model_shader"] = model_shader;
  m_shader_location_cache["depth_shader"] = depth_shader;
  m_shader_location_cache["hdr_shader"] = hdr_shader;

  main_shader->bind();
  main_shader->set_uniform1i("uAtlas", ATLAS_TEXTURE_SLOT);
  main_shader->set_uniform1i("uShadowMap", SHADOW_MAP_TEXTURE_SLOT);
  main_shader->unbind();

  model_shader->bind();
  model_shader->set_uniform1i("texture_diffuse1", DIFFUSE_TEXTURE_SLOT);
  model_shader->unbind();

  hdr_shader->bind();
  hdr_shader->set_uniform1i("uHdrTexture", HDR_TEXTURE_SLOT);
  hdr_shader->unbind();
}

void Shader::bind_shader_id(GLint id) { glUseProgram(id); }

// Activates the Shader Program
void Shader::bind() const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0); }

// Deletes the Shader Programs
void Shader::delete_shaders() {
  for (auto &it : m_shader_location_cache) {
    glDeleteProgram(it.second->get_id());
    delete it.second;
  }
}

GLint Shader::get_uniform(const std::string &name) {
  if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
    return m_uniform_location_cache[name];

  GLint loc = glGetUniformLocation(m_id, name.c_str());
  if (loc == -1) {
    //  MC_WARN("Uniform: {}, does not exist...", name.c_str());
  } else
    m_uniform_location_cache[name] = loc;

  return loc;
}

Shader *Shader::get_shader(const std::string &name) {
  if (m_shader_location_cache.find(name) != m_shader_location_cache.end())
    return m_shader_location_cache[name];

  else
    MC_ERROR("Couldn't find shader name: {}", name.c_str());

  return nullptr;
}

void Shader::set_uniform_vec4f(const std::string &uni_name,
                               const glm::vec4 &vector) {
  glUniform4f(get_uniform(uni_name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::set_uniform_vec3f(const std::string &uni_name,
                               const glm::vec3 &vector) {
  glUniform3f(get_uniform(uni_name), vector.x, vector.y, vector.z);
}

void Shader::set_uniform1f(const std::string &uni_name, const float val) {
  glUniform1f(get_uniform(uni_name), val);
}

void Shader::set_uniform1i(const std::string &uni_name, const int val) {
  glUniform1i(get_uniform(uni_name), val);
}

// Checks if the different Shaders have compiled properly
void Shader::compile_errors(unsigned int shader, const char *type) {
  // Stores status of compilation
  GLint has_compiled;
  // Character array to store error message in
  char info_log[1024];
  if (std::string(type) != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &has_compiled);
    if (has_compiled == GL_FALSE) {
      glGetShaderInfoLog(shader, 1024, NULL, info_log);
      MC_ERROR("Shader {} failed to compile: {}", type, info_log);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &has_compiled);
    if (has_compiled == GL_FALSE) {
      glGetProgramInfoLog(shader, 1024, NULL, info_log);
      MC_ERROR("Shader {} failed to link: {}", type, info_log);
    }
  }
}

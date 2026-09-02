#include "Shader.h"
#include "Log.h"
#include "util.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp> // for value_ptr

std::unordered_map<std::string, Shader *> Shader::m_ShaderLocationCache = {};

// Constructor that build the Shader Program from 2 different shaders
Shader::Shader(const std::string &filepath) : m_UniformLocationCache() {

  // Read vertexFile and fragmentFile
  ShaderProgramSource shaderCode = get_file_contents(filepath);

  // Convert the shader source strings into character arrays
  const char *vertexSource = shaderCode.VertexSource.c_str();
  const char *fragmentSource = shaderCode.FragmentSource.c_str();

  MC_DEBUG("Vertex Shader:\n {}", vertexSource);
  MC_DEBUG("Fragment Shader:\n {}", fragmentSource);

  // Create Vertex Shader Object and get its reference
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Attach Vertex Shader source to the Vertex Shader Object
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  // Compile the Vertex Shader into machine code
  glCompileShader(vertexShader);
  compileErrors(vertexShader, "VERTEX");

  // Create Fragment Shader Object and get its reference
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  // Attach Fragment Shader source to the Fragment Shader Object
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  // Compile the Vertex Shader into machine code
  glCompileShader(fragmentShader);
  compileErrors(fragmentShader, "FRAGMENT");

  // Create Shader Program Object and get its reference
  m_ID = glCreateProgram();

  // Attach the Vertex and Fragment Shaders to the Shader Program
  glAttachShader(m_ID, vertexShader);
  glAttachShader(m_ID, fragmentShader);

  glLinkProgram(m_ID);
  compileErrors(m_ID, "PROGRAM");

  glValidateProgram(m_ID);

  // Delete the now useless Vertex and Fragment Shader objects
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
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

void Shader::SetUniformMat4f(const std::string &uni_name,
                             const glm::mat4 &matrix) {
  // obv id, then num of matrix's which is 1, then whether or not we have a row
  // major matrix or column to transpose, since its col we dont
  glUniformMatrix4fv(getUniform(uni_name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniformVec3Array(const std::string &uni_name,
                                 const std::vector<glm::vec3> &values) {
  if (values.empty())
    return;

  glUniform3fv(getUniform(uni_name), static_cast<GLsizei>(values.size()),
               glm::value_ptr(values.front()));
}

void Shader::InitShaders() {
  const auto shaderPath = getResourcePath() / "shaders";

  Shader *main_shader = new Shader((shaderPath / "shader.glsl").string());
  Shader *model_shader = new Shader((shaderPath / "model.glsl").string());

  m_ShaderLocationCache["main_shader"] = main_shader;
  m_ShaderLocationCache["model_shader"] = model_shader;

  main_shader->Bind();

  main_shader->SetUniform1i("tex", 0);

  main_shader->Unbind();

  model_shader->Bind();
  model_shader->SetUniform1i("texture_diffuse1", 1);
  model_shader->Unbind();
}

void Shader::BindShaderID(GLint ID) { glUseProgram(ID); }

// Activates the Shader Program
void Shader::Bind() const { glUseProgram(m_ID); }
void Shader::Unbind() const { glUseProgram(0); }

// Deletes the Shader Programs
void Shader::DeleteShaders() {
  for (auto &it : m_ShaderLocationCache) {
    glDeleteProgram(it.second->GetID());
    delete it.second;
  }
}

GLint Shader::getUniform(const std::string &name) {
  if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    return m_UniformLocationCache[name];

  GLint loc = glGetUniformLocation(m_ID, name.c_str());
  if (loc == -1)
    MC_WARN("Uniform: {}, does not exist...", name.c_str());
  else
    m_UniformLocationCache[name] = loc;

  return loc;
}

Shader *Shader::getShader(const std::string &name) {
  if (m_ShaderLocationCache.find(name) != m_ShaderLocationCache.end())
    return m_ShaderLocationCache[name];

  else
    MC_ERROR("Couldn't find shader name: {}", name.c_str());

  return nullptr;
}

void Shader::SetUniformVec4f(const std::string &uni_name,
                             const glm::vec4 &vector) {
  glUniform4f(getUniform(uni_name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetUniformVec3f(const std::string &uni_name,
                             const glm::vec3 &vector) {
  glUniform3f(getUniform(uni_name), vector.x, vector.y, vector.z);
}

void Shader::SetUniform1f(const std::string &uni_name, const float val) {
  glUniform1f(getUniform(uni_name), val);
}

void Shader::SetUniform1i(const std::string &uni_name, const int val) {
  glUniform1i(getUniform(uni_name), val);
}

// Checks if the different Shaders have compiled properly
void Shader::compileErrors(unsigned int shader, const char *type) {
  // Stores status of compilation
  GLint hasCompiled;
  // Character array to store error message in
  char infoLog[1024];
  if (std::string(type) != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
    if (hasCompiled == GL_FALSE) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      MC_ERROR("Shader {} failed to compile: {}", type, infoLog);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
    if (hasCompiled == GL_FALSE) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      MC_ERROR("Shader {} failed to link: {}", type, infoLog);
    }
  }
}

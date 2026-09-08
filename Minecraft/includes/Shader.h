#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <cerrno>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ShaderProgramSource {
  std::string vertex_source;
  std::string fragment_source;
};

ShaderProgramSource get_file_contents(const std::string &filename);

class Shader {

public:
  GLuint m_id;
  Shader() {}
  Shader(const std::string &filename);
  static void delete_shaders();
  static void init_shaders();

  static void bind_shader_id(GLint id);
  void set_uniform1f(const std::string &uni_name, const float val);
  void set_uniform1i(const std::string &uni_name, const GLint val);
  void set_uniform_mat4f(const std::string &uni_name, const glm::mat4 &matrix);
  void set_uniform_vec3f(const std::string &uni_name, const glm::vec3 &vector);
  void set_uniform_vec4f(const std::string &uni_name, const glm::vec4 &vector);
  void set_uniform_vec3_array(const std::string &uni_name,
                           const std::vector<glm::vec3> &values);
  void set_uniform_vec3_array(const std::string &uni_name, const float *base_ptr,
                           size_t count);
  ;

  GLint get_uniform(const std::string &uni_name);
  static Shader *get_shader(const std::string &shader_name);

  void bind() const;
  void unbind() const;
  void compile_errors(unsigned int shader, const char *type);

  inline GLint get_id() { return m_id; };

private:
  std::unordered_map<std::string, GLint> m_uniform_location_cache;
  static std::unordered_map<std::string, Shader *> m_shader_location_cache;
};

#endif

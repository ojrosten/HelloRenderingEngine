////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

namespace avocet::opengl {
  namespace {
    void check_compilation_success(GLuint shaderID, std::string_view shaderType) {
      check_success(shaderID, shaderType, "compilation", GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
    }
  }

  void check_linking_success(GLuint shaderID) {
    check_success(shaderID, "program", "linking", GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
  }

  [[nodiscard]]
  std::string to_string(shader_species species) {
    using enum shader_species;
    switch(species) {
    case vertex:   return "vertex";
    case fragment: return "fragment";
    }

    throw std::runtime_error{"shader_species: unexpected value"};
  }

  shader_compiler::shader_compiler(shader_species species, std::string_view source)
    : m_Resource{species}
  {
    const auto index{m_Resource.handle().index()};
    const auto data{source.data()};
    glShaderSource(index, 1, &data, nullptr);
    glCompileShader(index);
    check_compilation_success(index, to_string(species));
  }

  shader_program::shader_program(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) {
    shader_compiler vertexShader{shader_species::vertex, vertexShaderSource}, fragmentShader{shader_species::fragment, fragmentShaderSource};

    const auto index{m_Resource.handle().index()};
    glAttachShader(index, vertexShader.resource().handle().index());
    glAttachShader(index, fragmentShader.resource().handle().index());
    glLinkProgram(index);
    check_linking_success(index);
  }
}
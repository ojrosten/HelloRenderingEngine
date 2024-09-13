////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

#include <format>
#include <stdexcept>

namespace avocet::opengl {
  namespace {
    template<class GetStatus>
    GLint check_status(const resource_handle& handle, GLenum status, GetStatus getStatus) {
      GLint success{};
      getStatus(handle.index(), status, &success);
      return success;
    }

    template<class GetStatus, class GetInfoLog>
    void check_success(const resource_handle& handle, std::string_view name, std::string_view buildStage, GLenum status, GetStatus getStatus, GetInfoLog getInfoLog) {
      if(!check_status(handle, status, getStatus)) {
        GLchar infoLog[512]{};
        getInfoLog(handle.index(), 512, nullptr, infoLog);
        throw std::runtime_error{std::format("error: {} shader - {} failed\n{}\n", name, buildStage, infoLog)};
      }
    }

    void check_compilation_success(const shader_resource& resource, std::string_view shaderType) {
      check_success(resource.handle(), shaderType, "compilation", GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
    }

    void check_linking_success(const shader_program_resource& resource) {
      check_success(resource.handle(), "program", "linking", GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
    }
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
    check_compilation_success(m_Resource, to_string(species));
  }

  shader_program::shader_program(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) {
    shader_compiler vertexShader{shader_species::vertex, vertexShaderSource}, fragmentShader{shader_species::fragment, fragmentShaderSource};

    const auto index{m_Resource.handle().index()};
    glAttachShader(index, vertexShader.resource().handle().index());
    glAttachShader(index, fragmentShader.resource().handle().index());
    glLinkProgram(index);
    check_linking_success(m_Resource);
  }
}
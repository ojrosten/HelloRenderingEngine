////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ResourceHandle.hpp"

#include <string>

namespace avocet::opengl {

  enum class shader_species : GLenum { vertex = GL_VERTEX_SHADER, fragment = GL_FRAGMENT_SHADER };

  class shader_resource {
    resource_handle m_Handle{};
  public:
    explicit shader_resource(shader_species species)
      : m_Handle{glCreateShader(static_cast<GLenum>(species))}
    {}

    shader_resource(shader_resource&&) noexcept = default;

    shader_resource& operator=(shader_resource&&) noexcept = default;

    ~shader_resource() { glDeleteShader(m_Handle.index()); }

    [[nodiscard]]
    friend bool operator==(const shader_resource&, const shader_resource&) noexcept = default;

    [[nodiscard]]
    const resource_handle& handle() const noexcept { return m_Handle; }
  };

  class shader_compiler {
    shader_resource m_Resource;
  public:
    shader_compiler(shader_species species, std::string_view source);

    [[nodiscard]]
    friend bool operator==(const shader_compiler&, const shader_compiler&) noexcept = default;

    [[nodiscard]]
    const shader_resource& resource() const noexcept { return m_Resource; }
  };

  class shader_program_resource {
    resource_handle m_Handle{};
  public:
    shader_program_resource()
      : m_Handle{glCreateProgram()}
    {}

    shader_program_resource(shader_program_resource&&) noexcept = default;

    shader_program_resource& operator=(shader_program_resource&&) noexcept = default;

    ~shader_program_resource() { glDeleteProgram(m_Handle.index()); }

    [[nodiscard]]
    friend bool operator==(const shader_program_resource&, const shader_program_resource&) noexcept = default;

    [[nodiscard]]
    const resource_handle& handle() const noexcept { return m_Handle; }
  };

  class shader_program {
    shader_program_resource m_Resource{};

  public:
    shader_program(std::string_view vertexShaderSource, std::string_view fragmentShaderSource);

    void use() { glUseProgram(m_Resource.handle().index()); }

    [[nodiscard]]
    friend bool operator==(const shader_program&, const shader_program&) noexcept = default;

    [[nodiscard]]
    const shader_program_resource& resource() const noexcept { return m_Resource; }
  };
}
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

  [[nodiscard]]
  std::string to_string(shader_species species);

  template<class Operations>
  class generic_shader_resource{
    resource_handle m_Handle{};
  public:
    template<class... Args>
    explicit(sizeof...(Args) == 1) generic_shader_resource(const Args&... args)
      : m_Handle{Operations::create(args...)}
    {}

    generic_shader_resource(generic_shader_resource&&) noexcept = default;

    generic_shader_resource& operator=(generic_shader_resource&&) noexcept = default;

    ~generic_shader_resource() { Operations::destroy(m_Handle); }

    [[nodiscard]]
    friend bool operator==(const generic_shader_resource&, const generic_shader_resource&) noexcept = default;

    [[nodiscard]]
    const resource_handle& handle() const noexcept { return m_Handle; }
  };

  struct shader_resource_operations {
    static resource_handle create(shader_species species) { return resource_handle{glCreateShader(static_cast<GLenum>(species))}; }

    static void destroy(const resource_handle& handle) { glDeleteShader(handle.index()); }
  };

  struct shader_program_operations {
    static resource_handle create() { return resource_handle{glCreateProgram()}; }

    static void destroy(const resource_handle& handle) { glDeleteProgram(handle.index()); }
  };

  using shader_resource = generic_shader_resource<shader_resource_operations>;
  using shader_program_resource = generic_shader_resource<shader_program_operations>;

  class shader_compiler {
    shader_resource m_Resource;
  public:
    shader_compiler(shader_species species, std::string_view source);

    [[nodiscard]]
    friend bool operator==(const shader_compiler&, const shader_compiler&) noexcept = default;

    [[nodiscard]]
    const shader_resource& resource() const noexcept { return m_Resource; }
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
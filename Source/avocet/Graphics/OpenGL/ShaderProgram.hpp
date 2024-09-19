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

  template<class T>
  inline constexpr bool has_destroy{
    requires (const resource_handle& h) { T::destroy(h); }
  };

  template<class T>
  inline constexpr bool has_create{
    requires (T& t) { { t.create() } -> std::same_as<resource_handle>; }
  };

  template<class T>
  inline constexpr bool is_lifecycle_v{has_create<T> && has_destroy<T> };

  template<class Lifecycle>
    requires is_lifecycle_v<Lifecycle>
  class generic_shader_resource{
    resource_handle m_Handle{};
  public:
    template<class... Args>
      requires std::constructible_from<Lifecycle, Args...>
    explicit(sizeof...(Args) == 1) generic_shader_resource(const Args&... args)
      : m_Handle{Lifecycle{args...}.create()}
    {}

    generic_shader_resource(generic_shader_resource&&) noexcept = default;

    generic_shader_resource& operator=(generic_shader_resource&&) noexcept = default;

    ~generic_shader_resource() { Lifecycle::destroy(m_Handle); }

    [[nodiscard]]
    friend bool operator==(const generic_shader_resource&, const generic_shader_resource&) noexcept = default;

    [[nodiscard]]
    const resource_handle& handle() const noexcept { return m_Handle; }
  };

  class shader_resource_lifecycle {
    shader_species m_Species;
  public:
    explicit shader_resource_lifecycle(shader_species species) : m_Species{species} {}

    resource_handle create() { return resource_handle{glCreateShader(static_cast<GLenum>(m_Species))}; }

    static void destroy(const resource_handle& handle) { glDeleteShader(handle.index()); }
  };

  struct shader_program_lifecycle {
    static resource_handle create() { return resource_handle{glCreateProgram()}; }

    static void destroy(const resource_handle& handle) { glDeleteProgram(handle.index()); }
  };

  using shader_resource = generic_shader_resource<shader_resource_lifecycle>;
  using shader_program_resource = generic_shader_resource<shader_program_lifecycle>;

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
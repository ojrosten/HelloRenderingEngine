////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

#include <format>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
        enum class shader_species : GLenum { vertex = GL_VERTEX_SHADER, fragment = GL_FRAGMENT_SHADER };

        [[nodiscard]]
        GLenum to_gl_enum(shader_species species) { return static_cast<GLenum>(species); }

        [[nodiscard]]
        std::string to_string(shader_species species) {
            using enum shader_species;
            switch(species) {
            case vertex:   return "vertex";
            case fragment: return "fragment";
            }

            throw std::runtime_error{std::format("shader_species: unexpected value {}", to_gl_enum(species))};
        }

        class shader_resource_lifecycle {
            shader_species m_Species;
        public:
            explicit shader_resource_lifecycle(shader_species species) : m_Species{species} {}

            resource_handle create() { return resource_handle{glCreateShader(to_gl_enum(m_Species))}; }

            static void destroy(const resource_handle& handle) { glDeleteShader(handle.index()); }
        };

        using shader_resource = generic_shader_resource<shader_resource_lifecycle>;

        [[nodiscard]]
        std::string to_build_stage(const shader_resource&) { return "compile"; }

        [[nodiscard]]
        std::string to_build_stage(const shader_program_resource&) { return "link"; }

        [[nodiscard]]
        GLenum to_build_status_flag(const shader_resource&) { return GL_COMPILE_STATUS; }

        [[nodiscard]]
        GLenum to_build_status_flag(const shader_program_resource&) { return GL_LINK_STATUS; }

        template<class T>
        constexpr bool is_gl_getter_v{std::is_invocable_v<T, GLuint, GLenum, GLint*>};

        template<class T>
        constexpr bool is_info_log_getter_v{std::is_invocable_v<T, GLuint, GLsizei, GLsizei*, GLchar*>};

        template<class T>
        constexpr bool is_resource_v{
            requires (const T & t) { { t.handle() } -> std::same_as<const resource_handle&>; }
        };

        template<class GetParameter>
            requires is_gl_getter_v<GetParameter>
        [[nodiscard]]
        GLint get_parameter_value(const resource_handle& handle, GLenum paramName, GetParameter getParameter) {
            GLint parameter{};
            getParameter(handle.index(), paramName, &parameter);
            return parameter;
        }

        template<class Resource, class GetParameter, class GetInfoLog>
            requires is_resource_v<Resource>&& is_gl_getter_v<GetParameter>&& is_info_log_getter_v<GetInfoLog>
        void check_success(const Resource& resource, std::string_view name, GetParameter getParameter, GetInfoLog getInfoLog) {
            const auto& handle{resource.handle()};
            if(!get_parameter_value(handle, to_build_status_flag(resource), getParameter)) {
                const GLint logLength{get_parameter_value(handle, GL_INFO_LOG_LENGTH, getParameter)};
                std::vector<GLchar> infoLog(logLength);
                getInfoLog(handle.index(), logLength, nullptr, infoLog.data());
                throw std::runtime_error{std::format("error: {} {} failed\n{}\n", name, to_build_stage(resource), infoLog.data())};
            }
        }

        void check_compilation_success(const shader_resource& resource, shader_species species) {
            check_success(resource, to_string(species) + " shader", glGetShaderiv, glGetShaderInfoLog);
        }

        void check_linking_success(const shader_program_resource& resource) {
            check_success(resource, "program", glGetProgramiv, glGetProgramInfoLog);
        }

        [[nodiscard]]
        std::string file_to_string(const fs::path& file) {
            if(std::ifstream ifile{file}) {
                return std::string(std::istreambuf_iterator<char>(ifile), {});
            }

            throw std::runtime_error{std::format("Unable to open file {}", file.generic_string())};
        }

        class shader_compiler {
            shader_resource m_Resource;
        public:
            shader_compiler(shader_species species, const std::filesystem::path& source) : m_Resource{species}
            {
                const auto index{m_Resource.handle().index()};
                const auto contents{file_to_string(source)};
                const auto data{contents.data()};
                glShaderSource(index, 1, &data, nullptr);
                glCompileShader(index);
                check_compilation_success(m_Resource, species);
            }

            [[nodiscard]]
            friend bool operator==(const shader_compiler&, const shader_compiler&) noexcept = default;

            [[nodiscard]]
            const shader_resource& resource() const noexcept { return m_Resource; }
        };

        [[nodiscard]]
        GLuint get_gl_index(const shader_compiler& c) noexcept { return c.resource().handle().index(); }
    }
 
    shader_program::shader_program(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource) {
        shader_compiler vertexShader{shader_species::vertex, vertexShaderSource}, fragmentShader{shader_species::fragment, fragmentShaderSource};

        const auto progIndex{m_Resource.handle().index()};
        glAttachShader(progIndex, get_gl_index(vertexShader));
        glAttachShader(progIndex, get_gl_index(fragmentShader));
        glLinkProgram(progIndex);
        check_linking_success(m_Resource);
    }
}
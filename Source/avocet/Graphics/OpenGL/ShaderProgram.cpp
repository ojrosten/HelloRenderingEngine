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

        template<class T>
        inline constexpr bool has_resource_checker_v{
            requires (const T & t) {
                { T::build_stage }   -> std::convertible_to<std::string_view>;
                { T::status_flag }   -> std::convertible_to<GLenum>;
                { t.name() }         -> std::convertible_to<std::string>;
            }
        };

        class resource_checker {
            using gl_param_getter    = void(*)(GLuint, GLenum, GLint*);
            using gl_info_log_getter = void(*)(GLuint, GLsizei, GLsizei*, GLchar*);

            const resource_handle& m_Handle;
            gl_param_getter m_ParamGetter;
            gl_info_log_getter m_LogGetter;

            static gl_param_getter validate(gl_param_getter g)       { return g ? g : throw std::runtime_error{"gl_param_getter: null function pointer"}; }
            static gl_info_log_getter validate(gl_info_log_getter g) { return g ? g : throw std::runtime_error{"gl_info_log_getter: null function pointer"}; }

            [[nodiscard]]
            GLint get_parameter_value(GLenum paramName) const {
                GLint parameter{};
                m_ParamGetter(m_Handle.index(), paramName, &parameter);
                return parameter;
            }

            [[nodiscard]]
            std::string get_log() const {
                const GLint logLength{get_parameter_value(GL_INFO_LOG_LENGTH)};
                std::string infoLog(logLength, ' ');
                m_LogGetter(m_Handle.index(), logLength, nullptr, infoLog.data());
                return infoLog;
            }
        protected:
            ~resource_checker() = default;
        public:
            resource_checker(const resource_handle& handle, gl_param_getter paramGetter, gl_info_log_getter logGetter)
                : m_Handle{handle}
                , m_ParamGetter{validate(paramGetter)}
                , m_LogGetter{validate(logGetter)}
            {}

            template<class Self>
                requires has_resource_checker_v<Self>
            void check(this Self&& self) {
                if(!self.get_parameter_value(self.status_flag)) {
                    const auto infoLog{self.get_log()};
                    throw std::runtime_error{std::format("error: {} {} failed\n{}\n", self.name(), self.build_stage, infoLog)};
                }
            }
        };

        class shader_resource_checker : public resource_checker {
            shader_species m_Species;
        public:
            constexpr static std::string_view build_stage{"compilation"};
            constexpr static GLenum status_flag{GL_COMPILE_STATUS};

            shader_resource_checker(const shader_resource& resource, shader_species species)
                : resource_checker{resource.handle(), glGetShaderiv, glGetShaderInfoLog}
                , m_Species{species}
            {}

            [[nodiscard]]
            std::string name() const { return to_string(m_Species) + " shader"; }
        };

        class shader_program_resource_checker : public resource_checker {
        public:
            constexpr static std::string_view build_stage{"linking"};
            constexpr static GLenum status_flag{GL_LINK_STATUS};

            shader_program_resource_checker(const shader_program_resource& resource)
                : resource_checker{resource.handle(), glGetProgramiv, glGetProgramInfoLog}
            {}

            [[nodiscard]]
            std::string name() const { return "program"; }
        };


        void check_compilation_success(const shader_resource& resource, shader_species species) { shader_resource_checker(resource, species).check(); }

        void check_linking_success(const shader_program_resource& resource) { shader_program_resource_checker{resource}.check(); }

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
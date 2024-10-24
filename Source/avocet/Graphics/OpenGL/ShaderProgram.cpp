////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"
#include "avocet/Graphics/OpenGL/Core.hpp"

#include <fstream>
#include <functional>

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
        enum class shader_species : GLenum { vertex = GL_VERTEX_SHADER, fragment = GL_FRAGMENT_SHADER };

        [[nodiscard]]
        std::string to_string(shader_species species) {
            using enum shader_species;
            switch(species) {
            case vertex:   return "vertex";
            case fragment: return "fragment";
            }

            throw std::runtime_error{std::format("shader_species - unexpected value: {}", static_cast<GLenum>(species))};
        }

        template<class T>
        inline constexpr bool has_checker_attributes_v{
            requires(const T & t) {
                { T::build_stage } -> std::convertible_to<std::string_view>;
                { T::status_flag } -> std::convertible_to<GLenum>;
                { t.name() }       -> std::convertible_to<std::string>;
            }
        };

        template<class T>
        inline constexpr bool castable_to_bool_v{
            requires (T& t) { static_cast<bool>(t); }
        };

        class shader_checker {
            using gl_param_getter    = std::function<void(GLuint, GLenum, GLint*)>;
            using gl_info_log_getter = std::function<void(GLuint, GLsizei, GLsizei*, GLchar*)>;

            const resource_handle& m_Handle;
            gl_param_getter m_ParamGetter;
            gl_info_log_getter m_InfoLogGetter;

            template<class T>
                requires castable_to_bool_v<T>
            [[nodiscard]]
            static T validate(std::string_view prefix, T t) { return t ? t : throw std::runtime_error{std::format("{} is null!", prefix)}; }

            [[nodiscard]]
            GLint get_parameter_value(GLenum paramName) const {
                GLint param{};
                gl_function{m_ParamGetter}(m_Handle.index(), paramName, &param);
                return param;
            }

            [[nodiscard]]
            std::string get_info_log() const {
                const GLint logLen{get_parameter_value(GL_INFO_LOG_LENGTH)};
                std::string info(logLen, ' ');
                gl_function{m_InfoLogGetter}(m_Handle.index(), logLen, nullptr, info.data());
                return info;
            }
        protected:
            ~shader_checker() = default;
        public:
            shader_checker(const resource_handle& h, gl_param_getter paramGetter, gl_info_log_getter logGetter)
                : m_Handle{h}
                , m_ParamGetter{validate("gl_param_getter", paramGetter)}
                , m_InfoLogGetter{validate("gl_info_log_getter", logGetter)}
            {}

            template<class Self>
                requires has_checker_attributes_v<Self>
            void check(this const Self& self) {
                if(!self.get_parameter_value(self.status_flag)) {
                    throw std::runtime_error{std::format("error {} {} failed\n{}\n", self.name(), self.build_stage, self.get_info_log())};
                }
            }
        };

        class shader_resource_lifecycle {
            shader_species m_Species;
        public:
            explicit shader_resource_lifecycle(shader_species species) : m_Species{species} {}

            [[nodiscard]]
            resource_handle create() { return resource_handle{gl_function{glCreateShader}(static_cast<GLenum>(m_Species))}; }

            static void destroy(const resource_handle& handle) { gl_function{glDeleteShader}(handle.index()); }
        };

        using shader_resource = generic_shader_resource<shader_resource_lifecycle>;

        class shader_compiler_checker : public shader_checker {
            shader_species m_Species;
        public:
            constexpr static std::string_view build_stage{"compilation"};
            constexpr static GLenum status_flag{GL_COMPILE_STATUS};

            shader_compiler_checker(const shader_resource& r, shader_species species)
                : shader_checker{r.handle(), glGetShaderiv, glGetShaderInfoLog}
                , m_Species{species}
            {}

            [[nodiscard]]
            std::string name() const { return to_string(m_Species) + " shader"; }
        };

        class shader_program_checker : public shader_checker {
        public:
            constexpr static std::string_view build_stage{"linking"};
            constexpr static GLenum status_flag{GL_LINK_STATUS};

            explicit shader_program_checker(const shader_program_resource& r)
                : shader_checker{r.handle(), glGetProgramiv, glGetProgramInfoLog}
            {}

            [[nodiscard]]
            std::string name() const { return "program"; }
        };

        [[nodiscard]]
        std::string read_to_string(const fs::path& file){
            if(std::ifstream ifile{file}) {
                return std::string(std::istreambuf_iterator<char>{ifile}, {});
            }

            throw std::runtime_error{std::format("Unable to open file {}", file.generic_string())};
        }

        class shader_compiler {
            shader_resource m_Resource;
        public:
            shader_compiler(shader_species species, const fs::path& sourceFile)
                : m_Resource{species}
            {
                const auto index{m_Resource.handle().index()};
                const auto source{read_to_string(sourceFile)};
                const auto data{source.data()};
                gl_function{glShaderSource}(index, 1, &data, nullptr);
                gl_function{glCompileShader}(index);
                shader_compiler_checker{m_Resource, species}.check();
            }

            [[nodiscard]]
            const shader_resource& resource() const noexcept { return m_Resource; }

            [[nodiscard]]
            friend bool operator==(const shader_compiler&, const shader_compiler&) noexcept = default;
        };

        class [[nodiscard]] shader_attacher {
            GLuint m_ProgIndex{}, m_ShaderIndex{};
        public:
            shader_attacher(const shader_program& program, const shader_compiler& shader)
                : m_ProgIndex{program.resource().handle().index()}
                , m_ShaderIndex{shader.resource().handle().index()}
            {
                gl_function{glAttachShader}(m_ProgIndex, m_ShaderIndex);
            }

            ~shader_attacher() { gl_function{glDetachShader}(m_ProgIndex, m_ShaderIndex); }
        };

        static_assert(has_shader_lifecycle_events_v<shader_resource_lifecycle>);
        static_assert(has_shader_lifecycle_events_v<shader_program_resource_lifecycle>);
    }

    shader_program::shader_program(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource) {
        shader_compiler
            vertexShader{shader_species::vertex, vertexShaderSource},
            fragmentShader{shader_species::fragment, fragmentShaderSource};

        const auto progIndex{m_Resource.handle().index()};

        {
            shader_attacher verteAttacher{*this, vertexShader}, fragmentAttacher{*this, fragmentShader};
            gl_function{glLinkProgram}(progIndex);
        }

        shader_program_checker{m_Resource}.check();
    }
}
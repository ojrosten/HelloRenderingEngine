////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

namespace avocet::opengl {
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

        template<class GetStatus>
        GLint check_status(GLuint shaderID, GLenum status, GetStatus getStatus) {
            GLint success{};
            getStatus(shaderID, status, &success);
            return success;
        }

        template<class GetStatus, class GetInfoLog>
        void check_success(GLuint shaderID, std::string_view name, std::string_view buildStage, GLenum status, GetStatus getStatus, GetInfoLog getInfoLog) {
            if(!check_status(shaderID, status, getStatus)) {
                GLchar infoLog[512]{};
                getInfoLog(shaderID, 512, nullptr, infoLog);
                throw std::runtime_error{std::format("ERROR::SHADER::{}::{}_FAILED\n{}\n", name, buildStage, infoLog)};
            }
        }

        void check_linking_success(GLuint programID) {
            check_success(programID, "PROGRAM", "LINKING", GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
        }

        void check_compilation_success(GLuint shaderID, shader_species species) {
            check_success(shaderID, to_string(species), "COMPILATION", GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
        }

        class shader_resource_lifecycle {
            shader_species m_Species;
        public:
            explicit shader_resource_lifecycle(shader_species species) : m_Species{species} {}

            [[nodiscard]]
            resource_handle create() { return resource_handle{glCreateShader(static_cast<GLenum>(m_Species))}; }

            static void destroy(const resource_handle& h) { glDeleteShader(h.index()); }
        };

        using shader_resource = generic_shader_resource<shader_resource_lifecycle>;

        class shader_compiler {
            shader_resource m_Resource;
        public:
            shader_compiler(shader_species species, std::string_view source)
                : m_Resource{species}
            {
                const auto index{m_Resource.handle().index()};
                const auto data{source.data()};
                glShaderSource(index, 1, &data, nullptr);
                glCompileShader(index);
                check_compilation_success(index, species);
            }

            [[nodiscard]]
            const shader_resource& resource() const noexcept { return m_Resource; }

            [[nodiscard]]
            friend bool operator==(const shader_compiler&, const shader_compiler&) noexcept = default;
        };

        class [[nodiscard]] shader_attacher{
            GLuint m_ProgIndex{}, m_ShaderIndex{};
        public:
            shader_attacher(const shader_program& prog, const shader_compiler& shader)
                : m_ProgIndex{prog.resource().handle().index()}
                , m_ShaderIndex{shader.resource().handle().index()}
            {
                glAttachShader(m_ProgIndex, m_ShaderIndex);
            }

            ~shader_attacher() { glDetachShader(m_ProgIndex, m_ShaderIndex); }
        };

        static_assert(has_lifecycle_events_v<shader_resource_lifecycle>);
        static_assert(has_lifecycle_events_v<shader_program_resource_lifecycle>);
    }

    shader_program::shader_program(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) {
        shader_compiler 
            vertexShader{shader_species::vertex, vertexShaderSource},
            fragmentShader{shader_species::fragment, fragmentShaderSource};

        const auto progIndex{m_Resource.handle().index()};

        {
            shader_attacher vertexShaderAttacher{*this, vertexShader}, fragShaderAttacher{*this, fragmentShader};
            glLinkProgram(progIndex);
        }
        check_linking_success(progIndex);
    }
}
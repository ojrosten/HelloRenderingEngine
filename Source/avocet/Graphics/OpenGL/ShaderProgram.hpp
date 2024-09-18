////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ResourceHandle.hpp"

#include <format>
#include <stdexcept>

namespace avocet::opengl {
    enum class shader_species : GLenum { vertex = GL_VERTEX_SHADER, fragment = GL_FRAGMENT_SHADER };

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

    void check_linking_success(GLuint programID);

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
        const resource_handle& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        friend bool operator==(const shader_resource&, const shader_resource&) noexcept = default;
    };

    class shader_compiler {
        shader_resource m_Resource;
    public:
        shader_compiler(shader_species species, std::string_view source);

        [[nodiscard]]
        const shader_resource& resource() const noexcept { return m_Resource; }

        [[nodiscard]]
        friend bool operator==(const shader_compiler&, const shader_compiler&) noexcept = default;
    };

}
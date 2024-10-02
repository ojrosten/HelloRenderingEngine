////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ResourceHandle.hpp"

#include <filesystem>

namespace avocet::opengl {
    struct shader_program_resource_lifecycle {
        [[nodiscard]]
        static resource_handle create() { return resource_handle{glCreateProgram()}; }

        static void destroy(const resource_handle& handle) { glDeleteProgram(handle.index()); }
    };

    using shader_program_resource = shader_resource<shader_program_resource_lifecycle>;

    class shader_program {
        shader_program_resource m_Resource;
    public:
        shader_program(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource);

        [[nodiscard]]
        const shader_program_resource& shader_resource() const noexcept { return m_Resource; }

        void use() { glUseProgram(m_Resource.handle().index()); }

        [[nodiscard]]
        friend bool operator==(const shader_program&, const shader_program&) noexcept = default;
    };
}
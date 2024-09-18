////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::string to_string(shader_species species) {
            using enum shader_species;
            switch(species) {
            case vertex:   return "vertex";
            case fragment: return "fragment";
            }

            throw std::runtime_error{std::format("shader_species - unexpected value: {}", static_cast<GLenum>(species))};
        }

        void check_compilation_success(GLuint shaderID, shader_species species) {
            check_success(shaderID, to_string(species), "COMPILATION", GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
        }
    }

    void check_linking_success(GLuint programID) {
        check_success(programID, "PROGRAM", "LINKING", GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
    }

    shader_compiler::shader_compiler(shader_species species, std::string_view source)
        : m_Resource{species}
    {
        const auto index{m_Resource.handle().index()};
        const auto data{source.data()};
        glShaderSource(index, 1, &data, NULL);
        glCompileShader(index);
        check_compilation_success(index, species);
    }
}
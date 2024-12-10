////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BuffersFreeTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/Graphics/OpenGL/Buffers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path buffers_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffers_free_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        test_vbo();
        test_vao();
        test_ebo();
    }

    template<class BufferObject, class T, std::size_t N>
    bool buffers_free_test::check_buffer_object(const reporter& description, const std::array<T, N>& buffer) {
        BufferObject bufferObject{};

        agl::gl_function{glBindBuffer}(GL_ARRAY_BUFFER, get_raw_index(bufferObject));
        agl::gl_function{glBufferData}(GL_ARRAY_BUFFER, sizeof(buffer), buffer.data(), GL_STATIC_DRAW);

        std::array<T, N> recoveredBuffer{};
        agl::gl_function{glGetBufferSubData}(GL_ARRAY_BUFFER, 0, sizeof(recoveredBuffer), recoveredBuffer.data());
        return check(equality, description, recoveredBuffer, buffer);
    }

    void buffers_free_test::test_vbo()
    {
        check_buffer_object<agl::vertex_buffer_object>("VBO Data", std::array<GLfloat, 4>{0.0, 1.0, 2.0, 3.0});
    }

    void buffers_free_test::test_vao()
    {
        agl::vertex_attribute_object vao{};

        agl::gl_function{glBindVertexArray}(get_raw_index(vao));
        agl::gl_function{glVertexAttribPointer}(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        GLuint param{};
        glGetVertexAttribIuiv(0, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &param);
        check(equality, "VAO Data", param, GLuint(3 * sizeof(GLfloat)));
    }

    void buffers_free_test::test_ebo()
    {
        check_buffer_object<agl::element_buffer_object>("EBO Data", std::array<GLfloat, 4>{0, 1, 3});
    }
}

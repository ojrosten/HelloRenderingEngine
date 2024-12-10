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

    void buffers_free_test::test_vbo()
    {
        agl::vertex_buffer_object vbo{};

        std::array<GLfloat, 4> buffer{0.0, 1.0, 2.0, 3.0};

        agl::gl_function{glBindBuffer}(GL_ARRAY_BUFFER, get_raw_index(vbo));
        agl::gl_function{glBufferData}(GL_ARRAY_BUFFER, sizeof(buffer), buffer.data(), GL_STATIC_DRAW);

        std::array<GLfloat, 4> recoveredBuffer{};
        agl::gl_function{glGetBufferSubData}(GL_ARRAY_BUFFER, 0, sizeof(recoveredBuffer), recoveredBuffer.data());
        check(equality, "VBO Data", recoveredBuffer, buffer);
    }

    void buffers_free_test::test_vao()
    {
    }

    void buffers_free_test::test_ebo()
    {
    }
}

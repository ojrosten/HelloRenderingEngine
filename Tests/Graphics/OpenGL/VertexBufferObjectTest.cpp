////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "VertexBufferObjectTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path vertex_buffer_object_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void vertex_buffer_object_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        std::vector<GLfloat> xBuffer{0.0, 1.0, 2.0, 3.0}, yBuffer{-4.0, -5.0, 7.0};

        using vbo = agl::vertex_buffer_object<GLfloat>;
        auto x = [&xBuffer]() { return vbo{xBuffer, agl::null_label}; };
        auto y = [&yBuffer]() { return vbo{yBuffer, agl::null_label}; };
        check(equivalence, "", x(), std::optional{xBuffer});
        check(equivalence, "", y(), std::optional{yBuffer});

        using opt_vec = std::optional<std::vector<GLfloat>>;
        check_semantics("", x(), y(), std::optional{xBuffer}, std::optional{yBuffer}, opt_vec{}, std::optional{xBuffer});
    }
}

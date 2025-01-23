////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "VertexBufferObjectTestingDiagnostics.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path vertex_buffer_object_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void vertex_buffer_object_false_negative_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        using vbo = agl::vertex_buffer_object<GLfloat>;
        using opt_vec = std::optional<std::vector<GLfloat>>;
        std::vector<GLfloat> xBuffer{0.0, 1.0, 2.0, 3.0};
        check(equivalence, "Wrong buffer data", vbo{xBuffer, agl::null_label}, opt_vec{{}});
        check(equivalence, "Buffer which should be null", vbo{xBuffer, agl::null_label}, opt_vec{});
    }
}

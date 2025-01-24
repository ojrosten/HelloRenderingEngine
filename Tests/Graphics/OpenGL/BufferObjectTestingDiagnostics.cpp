////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BufferObjectTestingDiagnostics.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path buffer_object_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_object_false_negative_test::run_tests()
    {
        execute<agl::vertex_buffer_object<GLfloat>>();
        execute<agl::element_buffer_object<GLubyte>>();
    }

    template<class Buffer>
    void buffer_object_false_negative_test::execute() {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        using T = Buffer::value_type;
        using opt_vec = std::optional<std::vector<T>>;
        std::vector<T> xBuffer{40, 41, 42, 43};
        check(equivalence, "Buffer which should be null", Buffer{xBuffer, agl::null_label}, opt_vec{});
        check(equivalence, "Too much buffer data",        Buffer{xBuffer, agl::null_label}, opt_vec{{}});
        check(equivalence, "Incorrect buffer data",       Buffer{xBuffer, agl::null_label}, opt_vec{{40, 42, 42, 43}});
    }
}

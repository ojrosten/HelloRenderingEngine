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
    namespace {
        template<class Buffer>
        void execute(move_only_false_negative_test& test) {
            using namespace curlew;
            glfw_manager manager{};
            auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

            using T = Buffer::value_type;
            using opt_vec = std::optional<std::vector<T>>;
            std::vector<T> xBuffer{40, 41, 42, 43};
            test.check(equivalence, test.report("Wrong amount of buffer data"), Buffer{xBuffer, agl::null_label}, opt_vec{{}});
            test.check(equivalence, test.report("Wrong buffer element"), Buffer{xBuffer, agl::null_label}, opt_vec{{40, 42, 42, 43}});
            test.check(equivalence, test.report("Buffer which should be null"), Buffer{xBuffer, agl::null_label}, opt_vec{});
        }
    }

    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path buffer_object_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_object_false_negative_test::run_tests()
    {
        execute<agl::vertex_buffer_object<GLfloat>>(*this);
        execute<agl::element_buffer_object<GLubyte>>(*this);
    }
}

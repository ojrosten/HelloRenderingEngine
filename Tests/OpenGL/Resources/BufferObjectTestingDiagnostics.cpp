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
    [[nodiscard]]
    std::filesystem::path buffer_object_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_object_false_negative_test::run_tests()
    {
        using namespace curlew;

        auto w{create_window({.hiding{window_hiding_mode::on}})};

        execute<agl::vertex_buffer_object<GLfloat>>(w);
        execute<agl::vertex_buffer_object<GLubyte>>(w);
    }

    template<class Buffer>
        requires is_gl_buffer_v<Buffer>
    void buffer_object_false_negative_test::execute(const curlew::opengl_window& w)
    {
        using T = Buffer::value_type;
        using opt_span = std::optional<std::span<const T>>;
        std::vector<T> buffer{40, 41, 42, 43};
        check(equivalence, "Buffer which should be null", Buffer{w.context(), buffer, agl::null_label}, opt_span{});
        check(equivalence, "Too much buffer data",        Buffer{w.context(), buffer, agl::null_label}, opt_span{std::vector<T>{}});
        check(equivalence, "Incorrect buffer data",       Buffer{w.context(), buffer, agl::null_label}, opt_span{std::vector<T>{40, 42, 42, 43}});
        check(equivalence, "Not enough buffer data",      Buffer{w.context(), buffer, agl::null_label}, opt_span{std::vector<T>{40, 41, 42, 43, 44}});
    }
}

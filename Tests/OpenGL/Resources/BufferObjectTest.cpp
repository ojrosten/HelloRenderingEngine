////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BufferObjectTest.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path buffer_object_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_object_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        execute<agl::vertex_buffer_object<GLfloat>>(w);
        execute<agl::element_buffer_object<GLubyte>>(w);
    }

    template<class Buffer>
        requires is_gl_buffer_v<Buffer>
    void buffer_object_test::execute(const curlew::window& w)
    {
        using T = Buffer::value_type;

        const std::vector<T> xBuffer{0, 1, 2, 4}, yBuffer{5, 6, 7};
        using opt_span = std::optional<std::span<const T>>;

        check_semantics("", Buffer{xBuffer, agl::null_label}, Buffer{yBuffer, agl::null_label}, opt_span{xBuffer}, opt_span{yBuffer}, opt_span{}, opt_span{xBuffer});
    }
}

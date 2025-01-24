////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BufferObjectTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

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

        execute<agl::vertex_buffer_object<GLfloat>>();
        execute<agl::element_buffer_object<GLubyte>>();
    }

    template<class Buffer>
        requires is_gl_buffer_v<Buffer>
    void buffer_object_test::execute()
    {
        using T = Buffer::value_type;
        std::vector<T> xBuffer{0, 1, 2, 3}, yBuffer{4, 5, 6};

        auto x = [&xBuffer]() { return Buffer{xBuffer, agl::null_label}; };
        auto y = [&yBuffer]() { return Buffer{yBuffer, agl::null_label}; };
        check(equivalence, "", x(), std::optional{xBuffer});
        check(equivalence, "", y(), std::optional{yBuffer});

        using opt_vec = std::optional<std::vector<T>>;
        check_semantics("", x(), y(), std::optional{xBuffer}, std::optional{yBuffer}, opt_vec{}, std::optional{xBuffer});
    }

    [[nodiscard]]
    std::filesystem::path buffer_object_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_object_labelling_free_test::labelling_tests()
    {
        execute<agl::vertex_buffer_object<GLfloat>>();
        execute<agl::element_buffer_object<GLubyte>>();
    }

    template<class Buffer>
        requires is_gl_buffer_v<Buffer>
    void buffer_object_labelling_free_test::execute()
    {
        using T = Buffer::value_type;
        std::string label{"A nice buffer label"};
        Buffer buffer{std::vector<T>{}, label};
        check(equality, "", buffer.extract_label(), label);
    }
}

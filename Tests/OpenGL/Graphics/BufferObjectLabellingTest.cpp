////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BufferObjectLabellingTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path buffer_object_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_object_labelling_free_test::labelling_tests(const GladGLContext& ctx)
    {
        execute<agl::vertex_buffer_object<GLfloat>>(ctx);
        execute<agl::element_buffer_object<GLubyte>>(ctx);
    }

    template<class Buffer>
        requires is_gl_buffer_v<Buffer>
    void buffer_object_labelling_free_test::execute(const GladGLContext& ctx)
    {
        std::string label{"This is a nice label!"};
        Buffer buffer{ctx, {}, label};
        check(equality, "", buffer.extract_label(), label);
    }
}

////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BuffersLabellingFreeTest.hpp"
#include "avocet/Graphics/OpenGL/Buffers.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path buffers_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffers_labelling_free_test::labelling_tests()
    {
        namespace agl = avocet::opengl;

        {
            agl::vertex_buffer_object vbo{};
            const std::string vboLabel{"Hello VBO Label"};
            vbo.add_label(vboLabel);
            check_object_label("VBO Label", agl::object_identifier::buffer, vbo.get_handle(), vboLabel);
        }

        {
            agl::vertex_attribute_object vao{};
            const std::string vaoLabel{"Hello VAO Label"};
            vao.add_label(vaoLabel);
            check_object_label("VAO Label", agl::object_identifier::vertex_array, vao.get_handle(), vaoLabel);
        }

        {
            agl::element_buffer_object ebo{};
            const std::string eboLabel{"Hello EBO Label"};
            ebo.add_label(eboLabel);
            check_object_label("EBO Label", agl::object_identifier::buffer, ebo.get_handle(), eboLabel);
        }
    }
}

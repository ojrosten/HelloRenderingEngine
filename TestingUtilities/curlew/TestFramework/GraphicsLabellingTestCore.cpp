////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/TestFramework/GraphicsLabellingTestCore.hpp"

#include "avocet/Utilities/OpenGL/Casts.hpp"

namespace curlew {
    [[nodiscard]]
    std::string get_object_label(avocet::opengl::object_identifier identifier, const avocet::opengl::resource_handle& handle, std::size_t expectedSize) {
        namespace agl = avocet::opengl;
        std::string label(expectedSize + 1, ' ');
        agl::gl_function{glGetObjectLabel}(agl::to_gl_enum(identifier), handle.index(), agl::to_gl_sizei(label.size()), nullptr, label.data());
        if((label.back() == '\0') || (label.back() == ' ')) label.pop_back();

        return label;
    }
}
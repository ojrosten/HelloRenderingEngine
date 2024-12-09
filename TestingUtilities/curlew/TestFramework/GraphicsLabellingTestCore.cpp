////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/TestFramework/GraphicsLabellingTestCore.hpp"

#include "avocet/Utilities/OpenGL/Casts.hpp"

#include <algorithm>
#include <ranges>

namespace curlew {
    namespace agl = avocet::opengl;
 
    namespace {
        [[nodiscard]]
        GLsizei get_max_label_length() {
            const static GLsizei length{
                [](){
                    GLint param{};
                    agl::gl_function{glGetIntegerv}(GL_MAX_LABEL_LENGTH, &param);
                    return agl::to_gl_sizei(param);
                }()
            };

            return length;
        }
    }

    [[nodiscard]]
    std::string get_object_label(avocet::opengl::object_identifier identifier, const avocet::opengl::resource_handle& handle) {
        std::string label(get_max_label_length(), ' ');
        agl::gl_function{glGetObjectLabel}(agl::to_gl_enum(identifier), handle.index(), agl::to_gl_sizei(label.size()), nullptr, label.data());
        auto pos{std::ranges::find_if_not(std::views::reverse(label), [](auto c) { return c == ' '; }).base()};
        if((pos != label.begin()) && (*(pos-1) == '\0'))
            --pos;

        label.erase(pos, label.end());

        return label;
    }
}
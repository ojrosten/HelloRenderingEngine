////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/TestFramework/GraphicsLabellingTestCore.hpp"

#include "avocet/Utilities/OpenGL/Casts.hpp"

namespace curlew {
    namespace agl = avocet::opengl;

    namespace {
        [[nodiscard]]
        GLint get_max_label_length() {
            const static GLint length{
                [](){
                    GLint param{};
                    agl::gl_function{glGetIntegerv}(GL_MAX_LABEL_LENGTH, &param);
                    return param;
                }()
            };

            return length;
        }
    }

    [[nodiscard]]
    std::string get_object_label(avocet::opengl::object_identifier identifier, const avocet::opengl::resource_handle& handle) {
        std::string label(get_max_label_length(), ' ');
        GLsizei numChars{};
        agl::gl_function{glGetObjectLabel}(
                agl::to_gl_enum(identifier),
                handle.index(),
                agl::to_gl_sizei(label.size()),
                &numChars,
                label.data()
            );

        label.erase(std::ranges::next(label.begin(), numChars, label.end()), label.end());

        return label;
    }
}
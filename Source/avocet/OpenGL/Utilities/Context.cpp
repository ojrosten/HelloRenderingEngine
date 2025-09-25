////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Utilities/Context.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"

namespace avocet::opengl {
    [[nodiscard]]
    texture_unit decorated_context::get_max_combined_texture_units(const decorated_context_base& ctx) {
        GLint param{};
        gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &param);
        return texture_unit{static_cast<std::size_t>(param)};
    }
}
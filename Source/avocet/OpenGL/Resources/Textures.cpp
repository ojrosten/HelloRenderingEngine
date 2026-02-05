////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Resources/Textures.hpp"

namespace avocet::opengl {

    [[nodiscard]]
    GLint extract_texture_2d_param(const decorated_context& ctx, GLenum paramName) {
        GLint param{};
        gl_function{&GladGLContext::GetTexLevelParameteriv}(ctx, GL_TEXTURE_2D, 0, paramName, &param);
        return param;
    }

    void texture_2d_lifecycle_events::do_configure(contextual_resource_view h, const texture_2d_configurator& config) {
        const auto& ctx{h.context()};
        gl_function{&GladGLContext::PixelStorei}(ctx, GL_UNPACK_ALIGNMENT, to_ogl_alignment(config.data_view.row_alignment()));
    }
}
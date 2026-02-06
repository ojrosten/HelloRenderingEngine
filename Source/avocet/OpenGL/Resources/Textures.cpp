////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Resources/Textures.hpp"

namespace avocet::opengl {

    //void load_to_gpu(contextual_resource_view crv, const configurator& config);

    [[nodiscard]]
    GLint extract_texture_2d_param(const decorated_context& ctx, GLenum paramName) {
        GLint param{};
        gl_function{&GladGLContext::GetTexLevelParameteriv}(ctx, GL_TEXTURE_2D, 0, paramName, &param);
        return param;
    }

    void texture_2d_lifecycle_events::preliminary_configuration(contextual_resource_view crv, const texture_2d_configurator& config) {
        const auto& ctx{crv.context()};
        gl_function{&GladGLContext::PixelStorei}(ctx, GL_UNPACK_ALIGNMENT, to_ogl_alignment(config.data_view.row_alignment()));
    }

    /*void framebuffer_texture_2d_lifecycle_events::configure(contextual_resource_view crv, const configurator& config) {
        add_label(identifier, crv, config.label);
        preliminary_configuration(crv.context(), config);
        if(config.common_config.parameter_setter)
            config.common_config.parameter_setter();
    }

    void texture_2d_lifecycle_events::configure(contextual_resource_view crv, const configurator& config) {
        add_label(identifier, crv, config.label);
        load_to_gpu(crv.context(), config);
        if(config.common_config.parameter_setter)
            config.common_config.parameter_setter();
    }*/
}
////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Resources/Framebuffer.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::optional<std::string> to_error_string(GLenum status) {
            switch(status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                return "Undefined";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                return "Incomplete Attachment";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                return "Missing Attachment";
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                return "Incomplete Draw Buffer";
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                return "Incomplete Read Buffer";
            case GL_FRAMEBUFFER_UNSUPPORTED:
                return "Unsupported";
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                return "Incomplete Multisample";
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                return "ncomplete Layer Target";
            }

            return {};
        }

    }

    void framebuffer_object::check_framebuffer_status() {
        const auto status{gl_function{&GladGLContext::CheckFramebufferStatus}(this->context(), GL_FRAMEBUFFER)};
        if(const auto optError{to_error_string(status)}; optError)
            throw std::runtime_error{std::format("Frambuffer incomplete: {}", optError.value())};
    }
}
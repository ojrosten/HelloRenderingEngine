////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Utilities/ContextResolver.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::opengl {
    // Implementation of the forward-declared function
    GladGLContext* get_current_context() {
        return curlew::get_current_gl_context();
    }
}

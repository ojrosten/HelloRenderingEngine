////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "GraphicsTestCore.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

namespace curlew {
    [[nodiscard]]
    rendering_setup find_rendering_setup() {
        const static auto setup{glfw_manager::find_rendering_setup()};
        return setup;
    }
}
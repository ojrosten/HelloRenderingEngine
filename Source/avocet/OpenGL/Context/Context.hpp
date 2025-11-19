////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/ContextBase/ContextBase.hpp"
#include "avocet/OpenGL/ContextBase/GLFunction.hpp"

namespace avocet::opengl {
    class decorated_context : public decorated_context_base {
    public:
        using decorated_context_base::decorated_context_base;
    };
}
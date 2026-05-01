////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

namespace avocet::opengl {
    template<class>
    struct gl_getter;

    template<>
    struct gl_getter<GLint> {
        template<std::derived_from<context_base> Context>
        [[nodiscard]]
        GLint operator()(const Context& ctx, GLenum name) const {
            GLint param{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, name, &param);
            return param;
        }
    };

    template<gl_arithmetic T, std::derived_from<context_base> Context>
    [[nodiscard]]
    T get(const Context& ctx, GLenum name) {
        return gl_getter<T>{}(ctx, name);
    }
}
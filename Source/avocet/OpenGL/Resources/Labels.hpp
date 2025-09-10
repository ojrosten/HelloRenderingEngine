////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Resources/ResourceHandle.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/ObjectIdentifiers.hpp"

#include <optional>

namespace avocet::opengl {
    using optional_label = std::optional<std::string>;
    inline constexpr optional_label null_label{std::nullopt};

    inline void add_label(object_identifier identifier, const contextual_resource_handle& h, const optional_label& label) {
        if(label && object_labels_activated(h.context())) {
            const auto& str{label.value()};
            gl_function{&GladGLContext::ObjectLabel}(h.context(), to_gl_enum(identifier), get_index(h), to_gl_sizei(str.size()), str.data());
        }
    }

    [[nodiscard]]
    inline GLint get_max_label_length(const GladGLContext& ctx) {
        const static GLint length{
            [&ctx](){
                GLint param{};
                gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_LABEL_LENGTH, &param);
                return param;
            }()
        };

        return length;
    }

    [[nodiscard]]
    inline std::string get_object_label(avocet::opengl::object_identifier identifier, const avocet::opengl::contextual_resource_handle& handle) {
        const auto& ctx{handle.context()};
        std::string label(get_max_label_length(ctx), ' ');
        GLsizei numChars{};
        gl_function{&GladGLContext::GetObjectLabel}(
            ctx,
            to_gl_enum(identifier),
            get_index(handle),
            to_gl_sizei(label.size()),
            &numChars,
            label.data()
            );

        label.erase(std::ranges::next(label.begin(), numChars, label.end()), label.end());

        return label;
    }
}
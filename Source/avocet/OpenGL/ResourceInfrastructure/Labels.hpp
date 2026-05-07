////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/ResourceInfrastructure/ContextualResourceView.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"
#include "avocet/OpenGL/ResourceInfrastructure/ObjectIdentifiers.hpp"

#include <optional>

namespace avocet::opengl {
    using optional_label = std::optional<std::string>;
    inline constexpr optional_label null_label{std::nullopt};

    inline void add_label(object_identifier identifier, decorated_contextual_resource_view h, const optional_label& label) {
        if(label && (h.context().characteristics().object_labels_available() != object_labelling_available::no)) {
            const auto& str{label.value()};
            gl_function{&GladGLContext::ObjectLabel}(h.context(), to_gl_enum(identifier), get_index(h), to_gl_sizei(str.size()), str.data());
        }
    }

    [[nodiscard]]
    inline std::optional<GLint> get_max_label_length(const decorated_context& ctx) {
        if(ctx.characteristics().object_labels_available() == object_labelling_available::no)
            return std::nullopt;

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
    inline std::string get_object_label(avocet::opengl::object_identifier identifier, avocet::opengl::decorated_contextual_resource_view dcrv) {
        const auto& ctx{dcrv.context()};
        const auto optMaxLabelLen{get_max_label_length(ctx)};
        if(!optMaxLabelLen)
            return "";

        std::string label(optMaxLabelLen.value(), ' ');
        GLsizei numChars{};
        gl_function{&GladGLContext::GetObjectLabel}(
            ctx,
            to_gl_enum(identifier),
            get_index(dcrv),
            to_gl_sizei(label.size()),
            &numChars,
            label.data()
            );

        label.erase(std::ranges::next(label.begin(), numChars, label.end()), label.end());

        return label;
    }
}
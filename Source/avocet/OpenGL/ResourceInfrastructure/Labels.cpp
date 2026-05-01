////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/ResourceInfrastructure/Labels.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

namespace avocet::opengl {
    void add_label(object_identifier identifier, decorated_contextual_resource_view h, const optional_label& label) {
        if(label && h.context().debug_characteristics().object_labels_activated()) {
            const auto& str{label.value()};
            gl_function{&GladGLContext::ObjectLabel}(h.context(), to_gl_enum(identifier), get_index(h), checked_conversion_to<GLsizei>(str.size()), str.data());
        }
    }

    [[nodiscard]]
    std::string get_object_label(avocet::opengl::object_identifier identifier, characteristic_contextual_resource_view handle) {
        const auto& ctx{handle.context()};
        std::string label(ctx.characteristics().max_label_length(), ' ');
        GLsizei numChars{};
        gl_function{&GladGLContext::GetObjectLabel}(
            ctx,
            to_gl_enum(identifier),
            get_index(handle),
            checked_conversion_to<GLsizei>(label.size()),
            &numChars,
            label.data()
       );

        label.erase(std::ranges::next(label.begin(), numChars, label.end()), label.end());

        return label;
    }
}
////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/ResourceInfrastructure/Labels.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"
#include "avocet/OpenGL/Utilities/Messages.hpp"

namespace avocet::opengl {
    void add_label(object_identifier identifier, decorated_contextual_resource_view h, const optional_label& label) {
        if(label && (h.context().fundamental_characteristics().object_labels_available() != object_labelling_available::no)) {
            const auto& str{label.value()};
            gl_function{&GladGLContext::ObjectLabel}(h.context(), to_gl_underlying_value<GLenum>(identifier), get_index(h), checked_conversion_to<GLsizei>(str.size()), str.data());
        }
    }

    [[nodiscard]]
    std::string get_object_label(object_identifier identifier, characteristic_contextual_resource_view ccrv) {
        const auto& ctx{ccrv.context()};
        const auto optMaxLabelLen{ctx.characteristics().max_label_length()};
        if(!optMaxLabelLen)
            return "";

        std::string label(optMaxLabelLen.value(), ' ');
        GLsizei length{};
        gl_function{&GladGLContext::GetObjectLabel}(
            ctx,
            to_gl_underlying_value<GLenum>(identifier),
            get_index(ccrv),
            checked_conversion_to<GLsizei>(label.size()),
            &length,
            label.data()
        );

        return trim(label, length);
    }
}
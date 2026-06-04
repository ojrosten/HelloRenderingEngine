////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/CharacteristicContext.hpp"
#include "avocet/OpenGL/Context/GLGetters.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::optional<GLint> get_max_label_length(const decorated_context& ctx) {
            if (ctx.fundamental_characteristics().object_labels_available() == object_labelling_available::no)
                return std::nullopt;

            return get(ctx, int_names::max_label_length);
        }
    }

    context_characteristics::context_characteristics(const decorated_context& ctx)
        : m_Vendor{get(ctx, string_names::vendor)}
        , m_Renderer{get(ctx, string_names::renderer)}
        , m_MaxLabelLength{get_max_label_length(ctx)}
    {
    }
}
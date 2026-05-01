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

    void add_label(object_identifier identifier, decorated_contextual_resource_view h, const optional_label& label);

    [[nodiscard]]
    std::string get_object_label(avocet::opengl::object_identifier identifier, characteristic_contextual_resource_view handle);
}
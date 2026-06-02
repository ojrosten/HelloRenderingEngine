////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

namespace avocet::opengl {

    // In some sense, the parameter `Underlying` is superfluous since it
    // is simply the underlying type of the scoped enumeration. It is
    // nevertheless useful since this function is generally used when
    // supplying arguments to raw OpenGL functions. Being forced to
    // spell the underlying type makes it easier to audit if values of
    // the right type(s) are being used. THis scheme can therefore help
    // avoid unwitting implicit conversions at the point of OpenGL dispatch.
    template<gl_underlies_enum Underlying, class Enum>
        requires std::is_scoped_enum_v<Enum>
              && std::same_as<std::underlying_type_t<Enum>, Underlying>
    [[nodiscard]]
    constexpr Underlying to_gl_underlying_value(Enum e) noexcept {
        return to_underlying_value(e);
    }

}
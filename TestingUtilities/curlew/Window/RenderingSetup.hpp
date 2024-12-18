////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "sequoia/Core/Logic/Bitmask.hpp"

#include <string>

namespace curlew {
    enum class selectivity_flavour : uint64_t { none = 0, os = 1, renderer = 2, opengl_version = 4, build = 8 };
    enum class specificity_flavour : uint64_t { none = 0, os = 1, renderer = 2, opengl_version = 4, build = 8 };
}

inline namespace sequoia_bitmask {
    template<>
    struct as_bitmask<curlew::selectivity_flavour> : std::true_type {};

    template<>
    struct as_bitmask<curlew::specificity_flavour> : std::true_type {};
}

namespace curlew {
    [[nodiscard]]
    bool is_intel(std::string_view renderer);

    [[nodiscard]]
    bool is_amd(std::string_view renderer);

    [[nodiscard]]
    bool is_nvidia(std::string_view renderer);

    [[nodiscard]]
    bool is_mesa(std::string_view renderer);

    [[nodiscard]]
    bool is_apple(std::string_view renderer);

    [[nodiscard]]
    std::string rendering_setup_discriminator(selectivity_flavour selectivity);

    [[nodiscard]]
    std::string rendering_setup_discriminator(specificity_flavour specificity);

    [[nodiscard]]
    std::string rendering_setup_summary();
}
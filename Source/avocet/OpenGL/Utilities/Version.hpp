////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"

#include <string>

namespace avocet::opengl{
    struct opengl_version {
        std::size_t major{4}, minor{1};

        [[nodiscard]]
        friend auto operator<=>(const opengl_version&, const opengl_version&) noexcept = default;
    };

    [[nodiscard]]
    std::string get_vendor();

    [[nodiscard]]
    std::string get_renderer();

    [[nodiscard]]
    std::string get_opengl_version_string();

    [[nodiscard]]
    opengl_version extract_opengl_version();


    class opengl_context_index {
        std::uintptr_t m_Index{};
    public:
        opengl_context_index() noexcept = default;

        explicit opengl_context_index(std::uintptr_t i) noexcept
            : m_Index{i}
        {}

        [[nodiscard]]
        std::uintptr_t value() const noexcept { return m_Index; }

        [[nodiscard]]
        friend constexpr bool operator==(const opengl_context_index&, const opengl_context_index&) noexcept = default;
    };
}

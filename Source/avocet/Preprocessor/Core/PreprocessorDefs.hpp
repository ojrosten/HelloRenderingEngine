////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

namespace avocet {
    [[nodiscard]]
    constexpr bool has_ndebug() noexcept {
        #if defined(NDEBUG)
            return true;
        #else
            return false;
        #endif
    }

    [[nodiscard]]
    constexpr bool is_windows() noexcept {
         #if defined(_WIN32)
            return true;
        #else
            return false;
        #endif
    }

    [[nodiscard]]
    constexpr bool is_apple() noexcept {
        #if defined(__APPLE__)
            return true;
        #else
            return false;
        #endif
    }
}
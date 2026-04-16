////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <concepts>
#include <span>

namespace avocet {
    template<class From, std::size_t N, std::invocable<From> Fn, class To = std::invoke_result_t<Fn, From>>
    [[nodiscard]]
    std::array<To, N> to_array(std::span<const From, N> from, Fn fn) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
                return std::array<To, N>{fn(from[Is])...};
            }(std::make_index_sequence<N>{});
    }

    template<class From, std::size_t N, std::invocable<From> Fn, class To = std::invoke_result_t<Fn, From>>
    [[nodiscard]]
    std::array<To, N> to_array(const std::array<From, N>& from, Fn fn) {
        return to_array(std::span(from), std::move(fn));
    }

    template<class From, std::size_t N, std::invocable<From&> Fn, class To = std::invoke_result_t<Fn, From&>>
    [[nodiscard]]
    std::array<To, N> to_array(std::span<From, N> from, Fn fn) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
                return std::array<To, N>{fn(from[Is])...};
            }(std::make_index_sequence<N>{});
    }
}
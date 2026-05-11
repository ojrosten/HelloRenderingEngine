////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/DecoratedContext.hpp"

#include <optional>

namespace avocet::opengl {
    class characteristic_context : public decorated_context {
    public:
        template<class Loader>
        constexpr static bool is_loader_v{decorated_context::is_loader_v<Loader>};

        template<class Fn>
        constexpr static bool is_decorator_v{decorated_context::is_decorator_v<Fn>};

        template<class Loader, class Prologue, class Epilogue>
            requires is_loader_v<Loader>
                  && is_decorator_v<Prologue>
                  && is_decorator_v<Epilogue>
        characteristic_context(debugging_mode mode, Loader loader, Prologue prologue, Epilogue epilogue)
            : decorated_context{mode, std::move(loader), std::move(prologue), std::move(epilogue)}
        {}
    protected:
        ~characteristic_context() = default;

        characteristic_context(characteristic_context&&) noexcept = default;

        characteristic_context& operator=(characteristic_context&&) noexcept = default;
    private:
    };
}
////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/Context.hpp"

namespace avocet::opengl {
    class decorated_context : public context {
    public:
        template<class Fn>
        constexpr static bool is_decorator_v{std::is_invocable_r_v<void, Fn, const context&, const decorator_data&>};

        template<class Loader, class Prologue, class Epilogue>
            requires is_decorator_v<Prologue>
                  && is_decorator_v<Epilogue>
                  && std::is_invocable_r_v<GladGLContext, Loader, GladGLContext>
        decorated_context(debugging_mode mode, Loader loader, Prologue prologue, Epilogue epilogue)
            : context{mode, std::move(loader)}
            , m_Prologue{std::move(prologue)}
            , m_Epilogue{std::move(epilogue)}
        {}

        template<class Fn, class... Args>
            requires (!std::is_void_v<std::invoke_result_t<Fn, Args...>>)
        std::invoke_result_t<Fn, Args...> invoke(this const decorated_context& self, const decorator_data& data, Fn fn, Args... args) {
            if(self.m_Prologue) self.m_Prologue(self, data);
        
            const auto res{fn(args...)};
        
            if(self.m_Epilogue) self.m_Epilogue(self, data);
        
            return res;
        }
        
        template<class Fn, class... Args>
            requires std::is_void_v<std::invoke_result_t<Fn, Args...>>
        void invoke(this const decorated_context& self, const decorator_data& data, Fn fn, Args... args) {
            if(self.m_Prologue) self.m_Prologue(self, data);
        
            fn(args...);
        
            if(self.m_Epilogue) self.m_Epilogue(self, data);
        }
    protected:
        ~decorated_context() = default;

        decorated_context(decorated_context&&) noexcept = default;

        decorated_context& operator=(decorated_context&&) noexcept = default;
    private:
        using decorator_type = std::function<void(const context&, const decorator_data&)>;

        decorator_type m_Prologue{}, m_Epilogue{};
    };
}

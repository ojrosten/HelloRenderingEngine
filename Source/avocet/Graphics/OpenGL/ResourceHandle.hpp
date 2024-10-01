////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "glad/gl.h"

#include <utility>
#include <concepts>

namespace avocet::opengl {
    class resource_handle {
        GLuint m_Index{};
    public:
        resource_handle() noexcept = default;

        explicit resource_handle(GLuint index) noexcept : m_Index{index} {}

        resource_handle(resource_handle&& other) noexcept : m_Index{std::exchange(other.m_Index, 0)} {}

        resource_handle& operator=(resource_handle&& other) noexcept
        {
            std::ranges::swap(m_Index, other.m_Index);
            return *this;
        }

        [[nodiscard]]
        GLuint index() const noexcept { return m_Index; }

        [[nodiscard]]
        friend bool operator==(const resource_handle&, const resource_handle&) noexcept = default;
    };

    template<class T>
    inline constexpr bool has_lifecycle_events_v{
        requires(T & t, const resource_handle & handle) {
            { t.create() } -> std::same_as<resource_handle>;
            t.destroy(handle);
        }
    };

    template<class LifeEvents>
        requires has_lifecycle_events_v<LifeEvents>
    class resource {
        resource_handle m_Handle;
    public:
        template<class... Args>
            requires std::is_constructible_v<LifeEvents, Args...>
        explicit(sizeof...(Args) == 1) resource(const Args&... args)
            : m_Handle{LifeEvents{args...}.create()}
        {}

        ~resource() { LifeEvents::destroy(m_Handle); }

        resource(resource&&) noexcept = default;

        resource& operator=(resource&&) noexcept = default;

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        friend bool operator==(const resource&, const resource&) noexcept = default;
    };
}
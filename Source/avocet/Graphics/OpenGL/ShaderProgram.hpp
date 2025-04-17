////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/ResourceHandle.hpp"
#include "avocet/Graphics/OpenGL/Labels.hpp"

#include <filesystem>
#include <unordered_map>
#include <span>

namespace avocet::opengl {
    template<class T>
    inline constexpr bool has_shader_lifecycle_events_v{
        requires(T& t, const resource_handle& handle) {
            { t.create() } -> std::same_as<resource_handle>;
            T::destroy(handle);
        }
    };

    template<class LifeEvents>
        requires has_shader_lifecycle_events_v<LifeEvents>
    class generic_shader_resource {
        resource_handle m_Handle;
    public:
        template<class... Args>
            requires std::is_constructible_v<LifeEvents, Args...>
        explicit(sizeof...(Args) == 1) generic_shader_resource(const Args&... args)
            : m_Handle{LifeEvents{args...}.create()}
        {}

        ~generic_shader_resource() { LifeEvents::destroy(m_Handle); }

        generic_shader_resource(generic_shader_resource&&) noexcept = default;

        generic_shader_resource& operator=(generic_shader_resource&&) noexcept = default;

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        friend bool operator==(const generic_shader_resource&, const generic_shader_resource&) noexcept = default;
    };

    struct shader_program_resource_lifecycle {
        [[nodiscard]]
        static resource_handle create() { return resource_handle{glCreateProgram()}; }

        static void destroy(const resource_handle& handle) { glDeleteProgram(handle.index()); }
    };

    using shader_program_resource = generic_shader_resource<shader_program_resource_lifecycle>;

    class shader_program {
    public:
        shader_program(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource);

        [[nodiscard]]
        std::string extract_label() const { return get_object_label(object_identifier::program, m_Resource.handle()); }

        [[nodiscard]]
        const shader_program_resource& resource() const noexcept { return m_Resource; }

        void use() { glUseProgram(m_Resource.handle().index()); }

        [[nodiscard]]
        friend bool operator==(const shader_program&, const shader_program&) noexcept = default;
    private:
        shader_program_resource m_Resource;
    };
}
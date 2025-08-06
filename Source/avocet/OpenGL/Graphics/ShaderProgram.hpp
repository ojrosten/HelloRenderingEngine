////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Graphics/ResourceHandle.hpp"
#include "avocet/OpenGL/Graphics/Labels.hpp"

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
        static resource_handle create() { return resource_handle{gl_function{glCreateProgram}()}; }

        static void destroy(const resource_handle& handle) { gl_function{glDeleteProgram}(handle.index()); }
    };

    using shader_program_resource = generic_shader_resource<shader_program_resource_lifecycle>;

    struct string_hash {
        using is_transparent = void;

        [[nodiscard]]
        std::size_t operator()(const std::string& txt) const noexcept {
            return std::hash<std::string>{}(txt);
        }

        [[nodiscard]]
        std::size_t operator()(std::string_view txt) const noexcept {
            return std::hash<std::string_view>{}(txt);
        }
    };

    class shader_program {
    public:
        shader_program(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource);

        shader_program(shader_program&&) noexcept = default;

        shader_program& operator=(shader_program&&) noexcept = default;

        ~shader_program() { program_tracker::reset(m_Resource); }

        [[nodiscard]]
        std::string extract_label() const { return get_object_label(object_identifier::program, m_Resource.handle()); }

        void use() { program_tracker::utilize(m_Resource); }

        void set_uniform(std::string_view name, GLfloat val) {
            do_set_uniform(name, glUniform1f, val);
        }

        void set_uniform(std::string_view name, GLint val) {
            do_set_uniform(name, glUniform1i, val);
        }

        void set_uniform(std::string_view name, std::span<const GLfloat, 2> vals) {
            do_set_uniform(name, glUniform2f, vals[0], vals[1]);
        }

        [[nodiscard]]
        friend bool operator==(const shader_program&, const shader_program&) noexcept = default;
    private:
        shader_program_resource m_Resource;
        using map_t = std::unordered_map<std::string, GLint, string_hash, std::ranges::equal_to>;
        map_t m_Uniforms;

        [[nodiscard]]
        GLint extract_uniform_location(std::string_view name);

        template<class... Args>
        void do_set_uniform(std::string_view name, void(*glFn)(GLint, Args...), Args... args) {
            use();
            gl_function{glFn}(extract_uniform_location(name), args...);
        }

        class program_tracker {
            inline static GLuint st_Current{};
        public:
            static void utilize(const shader_program_resource& spr) {
                if(const auto index{spr.handle().index()}; index != st_Current) {
                    gl_function{glUseProgram}(index);
                    st_Current = index;
                }
            }

            static void reset(const shader_program_resource& spr) {
                if(spr.handle().index() == st_Current)
                    st_Current = 0;
            }
        };
    };
}
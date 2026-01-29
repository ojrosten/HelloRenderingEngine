////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Resources/ResourceHandle.hpp"
#include "avocet/OpenGL/EnrichedContext/ActivatingContext.hpp"
#include "avocet/OpenGL/Resources/Labels.hpp"

#include <filesystem>
#include <unordered_map>
#include <span>

namespace avocet::opengl {
    template<class T>
    inline constexpr bool has_shader_lifecycle_events_v{
        requires(T& t, contextual_resource_view handle) {
            { t.create(handle.context()) } -> std::same_as<contextual_resource_handle>;
            T::destroy(handle);
        }
    };

    template<class LifeEvents>
        requires has_shader_lifecycle_events_v<LifeEvents>
    class generic_shader_resource {
        contextual_resource_handle m_Handle;
    public:
        template<class... Args>
            requires std::is_constructible_v<LifeEvents, Args...>
        explicit(sizeof...(Args) == 0) generic_shader_resource(const decorated_context& ctx, const Args&... args)
            : m_Handle{LifeEvents{args...}.create(ctx)}
        {}

        ~generic_shader_resource() { LifeEvents::destroy(contextual_handle()); }

        generic_shader_resource(generic_shader_resource&&)            noexcept = default;
        generic_shader_resource& operator=(generic_shader_resource&&) noexcept = default;

        [[nodiscard]]
        contextual_resource_view contextual_handle() const noexcept { return m_Handle.begin()[0]; }

        [[nodiscard]]
        friend bool operator==(const generic_shader_resource&, const generic_shader_resource&) noexcept = default;
    };

    template<class LifeEvents>
    [[nodiscard]]
    GLuint get_index(const generic_shader_resource<LifeEvents>& gsr) noexcept { return get_index(gsr.contextual_handle()); }

    struct shader_program_resource_lifecycle {
        constexpr static auto identifier{object_identifier::program};

        [[nodiscard]]
        static contextual_resource_handle create(const decorated_context& ctx) {
            return contextual_resource_handle{ctx, std::array{gl_function{&GladGLContext::CreateProgram}(ctx)}};
        }

        static void use(contextual_resource_view crv) {
            gl_function{&GladGLContext::UseProgram}(crv.context(), crv.handle().index());
        }

        static void destroy(contextual_resource_view handle) { gl_function{&GladGLContext::DeleteProgram}(handle.context(), get_index(handle)); }
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
        shader_program(const activating_context& ctx, const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource);

        shader_program(shader_program&&) noexcept = default;

        shader_program& operator=(shader_program&&) noexcept = default;

        [[nodiscard]]
        std::string extract_label() const { return get_object_label(object_identifier::program, m_Resource.contextual_handle()); }

        void use(this const shader_program& self) { self.m_Context->use(shader_program_resource_lifecycle{}, self.m_Resource.contextual_handle()); }

        void set_uniform(std::string_view name, GLfloat val) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform1f}, val);
        }

        void set_uniform(std::string_view name, GLint val) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform1i}, val);
        }

        void set_uniform(std::string_view name, std::span<const GLfloat, 2> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform2f}, vals[0], vals[1]);
        }

        void set_uniform(std::string_view name, std::span<const GLfloat, 4> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform4f}, vals[0], vals[1], vals[2], vals[3]);
        }

        [[nodiscard]]
        friend bool operator==(const shader_program&, const shader_program&) noexcept = default;
    private:
        const activating_context* m_Context{};
        shader_program_resource m_Resource;
        using map_t = std::unordered_map<std::string, GLint, string_hash, std::ranges::equal_to>;
        map_t m_Uniforms;

        [[nodiscard]]
        GLint extract_uniform_location(std::string_view name);

        template<class... Args>
        void do_set_uniform(std::string_view name, gl_function<void(GLint, Args...)> fn, Args... args) {
            use();
            fn(m_Resource.contextual_handle().context(), extract_uniform_location(name), args...);
        }
    };
}
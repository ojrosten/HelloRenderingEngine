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
        requires(T& t, const contextual_handle& h) {
            { t.create(h.context()) } -> std::same_as<contextual_handle>;
            T::destroy(h);
        }
    };

    template<class LifeEvents>
        requires has_shader_lifecycle_events_v<LifeEvents>
    class generic_shader_resource {        
        contextual_handle m_ContextualHandle;
    public:
        template<class... Args>
            requires std::is_constructible_v<LifeEvents, Args...>
        explicit(sizeof...(Args) == 1) generic_shader_resource(const GladGLContext& ctx, const Args&... args)
            : m_ContextualHandle{LifeEvents{args...}.create(ctx)}
        {}

        ~generic_shader_resource() { LifeEvents::destroy(m_ContextualHandle); }

        generic_shader_resource(generic_shader_resource&&) noexcept = default;

        generic_shader_resource& operator=(generic_shader_resource&&) noexcept = default;

        [[nodiscard]]
        const contextual_handle& get_contextual_handle() const noexcept { return m_ContextualHandle; }

        [[nodiscard]]
        friend bool operator==(const generic_shader_resource&, const generic_shader_resource&) noexcept = default;
    };

    struct shader_program_resource_lifecycle {
        [[nodiscard]]
        static contextual_handle create(const GladGLContext& ctx) { return contextual_handle{ctx, gpu_resource_handle<GLuint>{gl_function{&GladGLContext::CreateProgram}(ctx)}}; }

        static void destroy(const contextual_handle& h) { gl_function{&GladGLContext::DeleteProgram}(h.context(), h.handle().index()); }
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
        shader_program(const GladGLContext& ctx, const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource);

        shader_program(shader_program&&) noexcept = default;

        shader_program& operator=(shader_program&&) noexcept = default;

        ~shader_program() { program_tracker::reset(m_Resource); }

        [[nodiscard]]
        std::string extract_label() const { return get_object_label(object_identifier::program, m_Resource.get_contextual_handle()); }

        void use() { program_tracker::utilize(m_Resource); }

        void set_uniform(std::string_view name, GLfloat val) {
            do_set_uniform(name, &GladGLContext::Uniform1f, val);
        }

        void set_uniform(std::string_view name, GLint val) {
            do_set_uniform(name, &GladGLContext::Uniform1i, val);
        }

        void set_uniform(std::string_view name, std::span<const GLfloat, 2> vals) {
            do_set_uniform(name, &GladGLContext::Uniform2f, vals[0], vals[1]);
        }

        [[nodiscard]]
        friend bool operator==(const shader_program&, const shader_program&) noexcept = default;
    private:
        shader_program_resource m_Resource;
        using map_t = std::unordered_map<std::string, GLint, string_hash, std::ranges::equal_to>;
        map_t m_Uniforms;

        [[nodiscard]]
        const GladGLContext& context() const noexcept { return m_Resource.get_contextual_handle().context(); }

        [[nodiscard]]
        GLint extract_uniform_location(std::string_view name);

        template<class R, class... Args>
        void do_set_uniform(std::string_view name, glad_ctx_ptr_to_mem_fn_ptr_type<R, GLint, Args...> pm, Args... args) {
            use();
            gl_function{pm}(context(), extract_uniform_location(name), args...);
        }

        class program_tracker {
            inline static thread_local GLuint st_Current{};
        public:
            static void utilize(const shader_program_resource& spr) {
                if(const auto index{spr.get_contextual_handle().handle().index()}; index != st_Current) {
                    const auto& context{spr.get_contextual_handle().context()};
                    gl_function{&GladGLContext::UseProgram}(context, index);
                    st_Current = index;
                }
            }

            static void reset(const shader_program_resource& spr) {
                if(spr.get_contextual_handle().handle().index() == st_Current)
                    st_Current = 0;
            }
        };
    };
}

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

    struct context_hash {
        using is_transparent = void;

        [[nodiscard]]
        std::size_t operator()(const opengl_context_index& context) const noexcept {
            return context.value();
        }
    };

    class shader_program {
    public:
        shader_program(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource, opengl_context_index context);

        shader_program(shader_program&&) noexcept = default;

        shader_program& operator=(shader_program&&) noexcept = default;

        ~shader_program() { m_Tracker.reset(m_Resource); }

        [[nodiscard]]
        std::string extract_label() const { return get_object_label(object_identifier::program, m_Resource.handle()); }

        void use(opengl_context_index context) { m_Tracker.utilize(context, m_Resource); }

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
        class program_tracker {
            inline static thread_local std::unordered_map<opengl_context_index, GLuint, context_hash, std::ranges::equal_to> st_ContextToCurrentProgram{};
           
            opengl_context_index m_CreationContext{};
        public:
            explicit program_tracker(opengl_context_index context)
                : m_CreationContext{context}
            {
                st_ContextToCurrentProgram.insert(std::pair{m_CreationContext, GLuint{}});
            }

            [[nodiscard]]
            opengl_context_index context() const noexcept { return m_CreationContext; }

            void utilize(opengl_context_index context, const shader_program_resource& spr) {
                // This will fail if
                // a. The context is different from the creation context
                // b. The requested context was never shared with the creation context
                // This could be fixed by propagating shared contexts for use().
                // Alternatively, the entire optimization could be removed, and only
                // retained for things like FBOs which aren't shareable. There could still
                // be an issue, though: calling use() on a shader program in a non-shared
                // context that happens to have a shader_program wrapping the same index.
                // Similar considerations apply to binding textures.
                //
                // Note that propagating shared contexts would not be very robust. A different
                // approach would be a boolean flag as to whether or not to even consider
                // shared contexts and then simply *assume*, if yes, that the requested context
                // is a shared one. Any errors would be on the head of clients...
                auto[iter, inserted]{st_ContextToCurrentProgram.insert(std::pair{context, spr.handle().index()})};
                if(inserted)
                    gl_function{glUseProgram}(iter->second);
                else if(const auto index{spr.handle().index()}; index != iter->second) {
                    gl_function{glUseProgram}(index);
                    iter->second = index;
                }
            }

            void reset(const shader_program_resource& spr) {
                auto found{st_ContextToCurrentProgram.find(m_CreationContext)};
                if(found == st_ContextToCurrentProgram.end())
                    throw std::runtime_error{std::format("shader_program::program_tracker: Unable to find context {}", m_CreationContext.value())};

                if(spr.handle().index() == found->second)
                    found->second = {};
            }
        };

        shader_program_resource m_Resource;
        using map_t = std::unordered_map<std::string, GLint, string_hash, std::ranges::equal_to>;
        map_t m_Uniforms;
        program_tracker m_Tracker;

        [[nodiscard]]
        GLint extract_uniform_location(std::string_view name);

        template<class... Args>
        void do_set_uniform(std::string_view name, void(*glFn)(GLint, Args...), Args... args) {
            use(m_Tracker.context());
            gl_function{glFn}(extract_uniform_location(name), args...);
        }
    };
}
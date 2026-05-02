////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/ResourceInfrastructure/Labels.hpp"
#include "avocet/OpenGL/Resources/GenericResource.hpp"

#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include <filesystem>
#include <map>
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
        explicit(sizeof...(Args) == 0) generic_shader_resource(const resourceful_context& ctx, const Args&... args)
            : m_Handle{LifeEvents{args...}.create(ctx)}
        {}

        ~generic_shader_resource() { LifeEvents::destroy(view()); }

        generic_shader_resource(generic_shader_resource&&) noexcept = default;

        generic_shader_resource& operator=(generic_shader_resource&&) noexcept = default;

        [[nodiscard]]
        contextual_resource_view view() const noexcept { return m_Handle.begin()[0]; }

        [[nodiscard]]
        friend bool operator==(const generic_shader_resource&, const generic_shader_resource&) noexcept = default;
    };

    template<class LifeEvents>
    [[nodiscard]]
    GLuint get_index(const generic_shader_resource<LifeEvents>& gsr) noexcept { return get_index(gsr.view()); }

    class shader_program_lifecyle_events {
        std::filesystem::path m_VertexShaderSource, m_FragmentShaderSource;
    public:
        constexpr static auto identifier{object_identifier::program};
        constexpr static auto caching_id{caching_identifier::program};

        struct configurator {
            optional_label label;
        };

        shader_program_lifecyle_events(std::filesystem::path vertexShaderSource, std::filesystem::path fragmentShaderSource)
            : m_VertexShaderSource{std::move(vertexShaderSource)}
            , m_FragmentShaderSource{std::move(fragmentShaderSource)}
        {}

        [[nodiscard]]
        contextual_resource_handle create(this const shader_program_lifecyle_events& self, const resourceful_context& ctx);

        static void destroy(decorated_contextual_resource_view crv) { gl_function{&GladGLContext::DeleteProgram}(crv.context(), get_index(crv)); }

        static void use(decorated_contextual_resource_view crv) {
            gl_function{ &GladGLContext::UseProgram }(crv.context(), get_index(crv));
        }

        static void configure(decorated_contextual_resource_view h, const configurator& config) {
            add_label(identifier, h, config.label);
        }

        friend bool operator==(const shader_program_lifecyle_events&, const shader_program_lifecyle_events&) noexcept = default;
    };

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

    class shader_program : public generic_resource<num_resources{1}, shader_program_lifecyle_events> {
    public:
        using base_type = generic_resource<num_resources{1}, shader_program_lifecyle_events>;

        shader_program(const resourceful_context& ctx, const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource);

        shader_program(shader_program&&) noexcept = default;

        shader_program& operator=(shader_program&&) noexcept = default;

        void use(this const shader_program& self) { self.do_utilize(); }

        void set_uniform(std::string_view name, GLfloat val) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform1f}, val);
        }

        void set_uniform(std::string_view name, std::span<const GLfloat, 2> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform2f}, vals[0], vals[1]);
        }

        void set_uniform(std::string_view name, std::span<const GLfloat, 3> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform3f}, vals[0], vals[1], vals[2]);
        }

        void set_uniform(std::string_view name, std::span<const GLfloat, 4> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform4f}, vals[0], vals[1], vals[2], vals[3]);
        }

        void set_uniform(std::string_view name, GLint val) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform1i}, val);
        }

        void set_uniform(std::string_view name, std::span<const GLint, 2> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform2i}, vals[0], vals[1]);
        }

        void set_uniform(std::string_view name, std::span<const GLint, 3> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform3i}, vals[0], vals[1], vals[2]);
        }

        void set_uniform(std::string_view name, std::span<const GLint, 4> vals) {
            do_set_uniform(name, gl_function{&GladGLContext::Uniform4i}, vals[0], vals[1], vals[2], vals[3]);
        }

        template<gl_arithmetic T>
        [[nodiscard]]
        T get_uniform(std::string_view name) {
            T val{};
            get_uniform(name, val);
            return val;
        }

        template<gl_arithmetic T, std::size_t N>
            requires (N >= 1) && (N <= 4)
        [[nodiscard]]
        std::array<T, N> get_uniform(std::string_view name) {
            std::array<T, N> vals{};
            get_uniform(name, std::span{vals});
            return vals;
        }

        void get_uniform(std::string_view name, GLfloat& val) {
            do_get_uniform(name, gl_function{&GladGLContext::GetUniformfv}, &val);
        }

        void get_uniform(std::string_view name, GLint& val) {
            do_get_uniform(name, gl_function{&GladGLContext::GetUniformiv}, &val);
        }

        template<std::size_t N>
        void get_uniform(std::string_view name, std::span<GLfloat, N> vals) {
            do_get_uniform(name, gl_function{&GladGLContext::GetUniformfv}, vals.data());
        }

        template<std::size_t N>
        void get_uniform(std::string_view name, std::span<GLint, N> vals) {
            do_get_uniform(name, gl_function{&GladGLContext::GetUniformiv}, vals.data());
        }

        [[nodiscard]]
        friend bool operator==(const shader_program&, const shader_program&) noexcept = default;
    private:
        using map_t = std::unordered_map<std::string, GLint, string_hash, std::ranges::equal_to>;
        map_t m_Uniforms;

        [[nodiscard]]
        GLint extract_uniform_location(std::string_view name);

        template<class... Args>
        void do_set_uniform(std::string_view name, gl_function<void(GLint, Args...)> fn, Args... args) {
            use();
            fn(context(), extract_uniform_location(name), args...);
        }

        template<gl_arithmetic T>
        void do_get_uniform(std::string_view name, gl_function<void(GLuint, GLint, T*)> fn, T* val) {
            use();
            fn(context(), get_index(contextual_handle(index<0>{})), extract_uniform_location(name), val);
        }
    };
}

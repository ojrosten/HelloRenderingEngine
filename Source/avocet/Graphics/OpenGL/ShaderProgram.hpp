////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/ResourceHandle.hpp"
#include "avocet/Graphics/OpenGL/Labels.hpp"

#include <array>
#include <filesystem>
#include <map>

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

    // HT https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
    template<size_t N>
    struct string_literal {
        consteval string_literal(const char(&str)[N]) {
            std::ranges::copy_n(str, N, value);
        }

        char value[N];
    };

    class shader_program {
        shader_program_resource m_Resource;
        std::map<std::string, GLint, std::ranges::less> m_Uniforms;

        [[nodiscard]]
        GLint extract_uniform_location(std::string_view name);

        template<string_literal Name>
        [[nodiscard]]
        GLint extract_uniform_location() {
            const static auto loc{gl_function{glGetUniformLocation}(resource().handle().index(), Name.value)};
            if(loc == -1)
                throw std::runtime_error{std::format("shader program {}: Uniform \"{}\" not found", extract_label(), Name.value)};

            return loc;
        }
    public:
        shader_program(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource);

        [[nodiscard]]
        std::string extract_label() const { return get_object_label(object_identifier::program, m_Resource.handle()); }

        [[nodiscard]]
        const shader_program_resource& resource() const noexcept { return m_Resource; }

        void use() { glUseProgram(m_Resource.handle().index()); }

        void set_uniform(std::string_view name, GLfloat val) { gl_function{glUniform1f}(extract_uniform_location(name), val); }

        void set_uniform(std::string_view name, const std::array<GLfloat, 2> val) { gl_function{glUniform2f}(extract_uniform_location(name), val[0], val[1]); }

        template<string_literal Name>
        void set_uniform(GLfloat val) { gl_function{glUniform1f}(extract_uniform_location<Name>(), val); }

        template<string_literal Name>
        void set_uniform(const std::array<GLfloat, 2>& val) { gl_function{glUniform2f}(extract_uniform_location<Name>(), val[0], val[1]); };

        [[nodiscard]]
        friend bool operator==(const shader_program&, const shader_program&) noexcept = default;
    };
}
////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Resources/ContextualResource.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include "avocet/Core/Formatting/Formatting.hpp"

#include "sequoia/FileSystem/FileSystem.hpp"

#include <fstream>
#include <functional>

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
        enum class shader_species : GLenum { vertex = GL_VERTEX_SHADER, fragment = GL_FRAGMENT_SHADER };

        [[nodiscard]]
        std::string make_program_label(const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource) {
            return std::format("{} / {}", sequoia::back(vertexShaderSource).string(), sequoia::back(fragmentShaderSource).string());
        }

        [[nodiscard]]
        std::string to_string(shader_species species) {
            using enum shader_species;
            switch(species) {
            case vertex:   return "vertex";
            case fragment: return "fragment";
            }

            throw std::runtime_error{error_message("shader_species", species)};
        }

        template<class T>
        inline constexpr bool has_checker_attributes_v{
            requires(const T & t) {
                { T::build_stage } -> std::convertible_to<std::string_view>;
                { T::status_flag } -> std::convertible_to<GLenum>;
                { t.name() }       -> std::convertible_to<std::string>;
            }
        };

        class shader_checker {
        public:
            using gl_param_getter    = gl_function<void(GLuint, GLenum, GLint*)>;
            using gl_info_log_getter = gl_function<void(GLuint, GLsizei, GLsizei*, GLchar*)>;

            shader_checker(decorated_contextual_resource_view crv, gl_param_getter paramGetter, gl_info_log_getter logGetter)
                : m_Handle{crv}
                , m_ParamGetter{paramGetter}
                , m_InfoLogGetter{logGetter}
            {}

            template<class Self>
                requires has_checker_attributes_v<Self>
            void check(this const Self& self) {
                if(!self.get_parameter_value(self.status_flag)) {
                    throw std::runtime_error{std::format("Error: {} {} failed\n{}\n", self.name(), self.build_stage, self.get_info_log())};
                }
            }
        protected:
            ~shader_checker() = default;
        private:
            decorated_contextual_resource_view m_Handle;
            gl_param_getter    m_ParamGetter;
            gl_info_log_getter m_InfoLogGetter;

            [[nodiscard]]
            GLint get_parameter_value(GLenum paramName) const {
                GLint param{};
                m_ParamGetter(m_Handle.context(), get_index(m_Handle), paramName, &param);
                return param;
            }

            [[nodiscard]]
            std::string get_info_log() const {
                const GLint logLen{get_parameter_value(GL_INFO_LOG_LENGTH)};
                std::string info(logLen, ' ');
                m_InfoLogGetter(m_Handle.context(), get_index(m_Handle), logLen, nullptr, info.data());
                return info;
            }
        };

        class shader_compiler_checker : public shader_checker {
            shader_species m_Species;
        public:
            constexpr static std::string_view build_stage{"compilation"};
            constexpr static GLenum status_flag{GL_COMPILE_STATUS};

            shader_compiler_checker(decorated_contextual_resource_view dcrv, shader_species species)
                : shader_checker{dcrv, gl_function{&GladGLContext::GetShaderiv}, gl_function{&GladGLContext::GetShaderInfoLog}}
                , m_Species{species}
            {}

            [[nodiscard]]
            std::string name() const { return std::format("{} shader", m_Species); }
        };

        class shader_program_checker : public shader_checker {
        public:
            constexpr static std::string_view build_stage{"linking"};
            constexpr static GLenum status_flag{GL_LINK_STATUS};

            explicit shader_program_checker(decorated_contextual_resource_view dcrv)
                : shader_checker{dcrv, gl_function{&GladGLContext::GetProgramiv}, gl_function{&GladGLContext::GetProgramInfoLog}}
            {}

            [[nodiscard]]
            std::string name() const { return "program"; }
        };

        [[nodiscard]]
        std::string read_to_string(const fs::path& file){
            if(fs::exists(file) && !fs::is_regular_file(file))
                throw std::runtime_error{std::format("Attempting to open something which isn't a file: {}", file.generic_string())};

            if(std::ifstream ifile{file}) {
                return std::string(std::istreambuf_iterator<char>{ifile}, {});
            }

            throw std::runtime_error{std::format("Unable to open file {}", file.generic_string())};
        }

        class shader_lifecycle_events {
            shader_species m_Species;
        public:
            constexpr static auto identifier{object_identifier::shader};

            struct configurator {
                fs::path source;
                optional_label label;
            };

            explicit shader_lifecycle_events(shader_species species) : m_Species{species} {}

            [[nodiscard]]
            contextual_resource_handle create(this const shader_lifecycle_events& self, const resourceful_context& ctx) {
                return contextual_resource_handle{ctx, std::array{gl_function{&GladGLContext::CreateShader}(ctx, to_gl_enum(self.m_Species))}};
            }

            static void destroy(decorated_contextual_resource_view crv) { gl_function{&GladGLContext::DeleteShader}(crv.context(), get_index(crv)); }

            static void use(decorated_contextual_resource_view) {}

            void configure(this const shader_lifecycle_events& self, decorated_contextual_resource_view dcrv, const configurator& config) {
                add_label(identifier, dcrv, config.label);

                const auto index{get_index(dcrv)};
                const auto source{read_to_string(config.source)};
                const auto data{source.data()};
                gl_function{&GladGLContext::ShaderSource}(dcrv.context(), index, 1, &data, nullptr);
                gl_function{&GladGLContext::CompileShader}(dcrv.context(), index);
                shader_compiler_checker{dcrv, self.m_Species}.check();
            }

            [[nodiscard]]
            friend bool operator==(const shader_lifecycle_events&, const shader_lifecycle_events&) noexcept = default;
        };

        class shader_compiler : public generic_resource<num_resources{1}, shader_lifecycle_events> {
        public:
            using base_type = generic_resource<num_resources{1}, shader_lifecycle_events>;

            shader_compiler(const resourceful_context& ctx, shader_species species, const fs::path& sourceFile)
                : base_type{ctx, shader_lifecycle_events{species}, {{sourceFile, null_label}}}
            {
            }

            [[nodiscard]]
            decorated_contextual_resource_view view() const noexcept { return contextual_handle(); }

            [[nodiscard]]
            friend bool operator==(const shader_compiler&, const shader_compiler&) noexcept = default;
        };

        class [[nodiscard]] shader_attacher {
            decorated_contextual_resource_view
                m_ProgView,
                m_ShaderView;
        public:
            shader_attacher(decorated_contextual_resource_view progView, decorated_contextual_resource_view shaderView)
                : m_ProgView{progView}
                , m_ShaderView{shaderView}
            {
                gl_function{&GladGLContext::AttachShader}(m_ProgView.context(), get_index(m_ProgView), get_index(m_ShaderView));
            }

            ~shader_attacher() { gl_function{&GladGLContext::DetachShader}(m_ProgView.context(), get_index(m_ProgView), get_index(m_ShaderView)); }
        };
    }

    [[nodiscard]]
    contextual_resource_handle shader_program_lifecyle_events::create(this const shader_program_lifecyle_events& self, const resourceful_context& ctx) {
        contextual_resource_handle crh{ctx, std::array{gl_function{&GladGLContext::CreateProgram}(ctx)}};

        shader_compiler
            vertexShader  {ctx, shader_species::vertex,   self.m_VertexShaderSource},
            fragmentShader{ctx, shader_species::fragment, self.m_FragmentShaderSource};

        decorated_contextual_resource_view progView{ctx, crh.begin()[0].handle()};

        {
            shader_attacher verteAttacher{progView, vertexShader.view()}, fragmentAttacher{progView, fragmentShader.view()};
            gl_function{&GladGLContext::LinkProgram}(ctx, get_index(progView));
        }

        shader_program_checker{progView}.check();

        return crh;
    }

    shader_program::shader_program(const resourceful_context& ctx, const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource)
        : base_type{ctx, shader_program_lifecyle_events{vertexShaderSource, fragmentShaderSource}, {{make_program_label(vertexShaderSource, fragmentShaderSource)}}}
    {
    }

    [[nodiscard]]
    GLint shader_program::extract_uniform_location(this const shader_program& self, std::string_view name) {
        if(auto found{self.m_Uniforms.find(name)}; found != self.m_Uniforms.end())
            return found->second;

        const auto location{gl_function{&GladGLContext::GetUniformLocation}(self.context(), get_index(self.contextual_handle()), name.data())};
        if(location == -1) {
            const bool labelled{self.context().debug_characteristics().object_labels_activated()};
            const std::string label{labelled ? self.extract_label() : "[unlabelled]"};

            throw std::runtime_error{std::format("shader_program {}: uniform \"{}\" not found", label, name)};
        }

        self.m_Uniforms.emplace(name, location);
        return location;
    }
}
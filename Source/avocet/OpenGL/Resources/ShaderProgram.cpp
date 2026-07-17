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
#include "avocet/OpenGL/Utilities/Messages.hpp"

#include "avocet/Core/Formatting/Formatting.hpp"

#include "sequoia/FileSystem/FileSystem.hpp"

#include <fstream>
#include <functional>

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
        enum class shader_species : GLenum { vertex = GL_VERTEX_SHADER, fragment = GL_FRAGMENT_SHADER };

        [[nodiscard]]
        std::string to_string(shader_species species) {
            using enum shader_species;
            switch(species) {
            case vertex:   return "vertex";
            case fragment: return "fragment";
            }

            throw std::runtime_error{error_message("shader_species", species)};
        }

        [[nodiscard]]
        optional_label make_program_label(object_labelling_available labelling, const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource) {
            if(labelling != object_labelling_available::no)
                return std::format("{} / {}", sequoia::back(vertexShaderSource).string(), sequoia::back(fragmentShaderSource).string());

            return std::nullopt;
        }

        [[nodiscard]]
        optional_label make_stage_label(object_labelling_available labelling, const std::filesystem::path& source) {
            if(labelling != object_labelling_available::no)
                return std::format("{}", sequoia::back(source).string());

            return std::nullopt;
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
                    throw std::runtime_error{std::format("Error: {} {} failed\n{}", self.name(), self.build_stage, self.get_info_log())};
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
                GLsizei length{};
                m_InfoLogGetter(m_Handle.context(), get_index(m_Handle), logLen, &length, info.data());

                return trim(info, length);
            }
        };

        class shader_stage_checker : public shader_checker {
            shader_species m_Species;
        public:
            constexpr static std::string_view build_stage{"compilation"};
            constexpr static GLenum status_flag{GL_COMPILE_STATUS};

            shader_stage_checker(decorated_contextual_resource_view stageView, shader_species species)
                : shader_checker{stageView, gl_function{&GladGLContext::GetShaderiv}, gl_function{&GladGLContext::GetShaderInfoLog}}
                , m_Species{species}
            {}

            [[nodiscard]]
            std::string name() const { return std::format("{} shader", m_Species); }
        };

        class shader_program_checker : public shader_checker {
        public:
            constexpr static std::string_view build_stage{"linking"};
            constexpr static GLenum status_flag{GL_LINK_STATUS};

            explicit shader_program_checker(decorated_contextual_resource_view progView)
                : shader_checker{progView, gl_function{&GladGLContext::GetProgramiv}, gl_function{&GladGLContext::GetProgramInfoLog}}
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

        class shader_stage_lifecycle_events {
            shader_species m_Species;
        public:
            constexpr static auto identifier{ object_identifier::shader};
            constexpr static auto caching_id{caching_identifier::not_applicable};

            struct configurator {
                std::filesystem::path source_file;
                optional_label label;
            };

            explicit shader_stage_lifecycle_events(shader_species species) : m_Species{species} {}

            [[nodiscard]]
            contextual_resource_handle create(this const shader_stage_lifecycle_events& self, const resourceful_context& ctx) {
                return {
                    ctx,
                    std::array{gl_function{&GladGLContext::CreateShader}(ctx, to_gl_underlying_value<GLenum>(self.m_Species))}
                };
            }

            static void destroy(decorated_contextual_resource_view stageView) {
                gl_function{&GladGLContext::DeleteShader}(stageView.context(), get_index(stageView));
            }

            void configure(this const shader_stage_lifecycle_events& self, decorated_contextual_resource_view stageView, const configurator& config) {
                add_label(identifier, stageView, config.label);

                const auto index{get_index(stageView)};
                const auto source{read_to_string(config.source_file)};
                const auto data{source.data()};
                const auto& ctx{stageView.context()};
                gl_function{&GladGLContext::ShaderSource }(ctx, index, 1, &data, nullptr);
                gl_function{&GladGLContext::CompileShader}(ctx, index);
                shader_stage_checker{stageView, self.m_Species}.check();
            }

            [[nodiscard]]
            friend constexpr bool operator==(const shader_stage_lifecycle_events&, const shader_stage_lifecycle_events&) noexcept = default;
        };

        class shader_stage : public generic_resource<num_resources{1}, shader_stage_lifecycle_events>
        {
        public:
            using generic_resource_type = generic_resource<num_resources{1}, shader_stage_lifecycle_events>;

            shader_stage(const resourceful_context& ctx, shader_species species, const fs::path& sourceFile)
                : generic_resource_type{ctx, shader_stage_lifecycle_events{species}, {sourceFile, make_stage_label(ctx.fundamental_characteristics().object_labels_available(), sourceFile)}}
            {
            }

            using generic_resource_type::contextual_handle_view;

            [[nodiscard]]
            friend bool operator==(const shader_stage&, const shader_stage&) noexcept = default;
        };

        class [[nodiscard]] shader_attacher {
            decorated_contextual_resource_view m_ProgView, m_StageView;
        public:
            shader_attacher(decorated_contextual_resource_view progView, decorated_contextual_resource_view stageView)
                : m_ProgView{progView}
                , m_StageView{stageView}
            {
                gl_function{&GladGLContext::AttachShader}(m_ProgView.context(), get_index(m_ProgView), get_index(m_StageView));
            }

            ~shader_attacher() { gl_function{&GladGLContext::DetachShader}(m_ProgView.context(), get_index(m_ProgView), get_index(m_StageView)); }
        };
    }

    void shader_program_lifecycle_events::configure(resourceful_contextual_resource_view progView, const configurator& config) {
        add_label(identifier, progView, config.label);

        const auto& ctx{progView.context()};

        shader_stage
            vertexShader  {ctx, shader_species::vertex,   config.vertex_shader},
            fragmentShader{ctx, shader_species::fragment, config.fragment_shader};

        {
            shader_attacher verteAttacher   {progView,   vertexShader.contextual_handle_view()},
                            fragmentAttacher{progView, fragmentShader.contextual_handle_view()};

            gl_function{&GladGLContext::LinkProgram}(ctx, get_index(progView));
        }

        shader_program_checker{progView}.check();
    }

    shader_program::shader_program(const resourceful_context& ctx, const std::filesystem::path& vertexShaderSource, const std::filesystem::path& fragmentShaderSource)
        : generic_resource_type{ctx, shader_program_lifecycle_events{}, {vertexShaderSource, fragmentShaderSource, make_program_label(ctx.fundamental_characteristics().object_labels_available(), vertexShaderSource, fragmentShaderSource)}}
    {
    }

    [[nodiscard]]
    GLint shader_program::extract_uniform_location(std::string_view name) {
        if(auto found{m_Uniforms.find(name)}; found != m_Uniforms.end())
            return found->second;

        const auto location{gl_function{&GladGLContext::GetUniformLocation}(this->context(), get_index(contextual_handle_view()), name.data())};
        if(location == -1) {
            const std::string label{extract_label()};
            const std::string labelToUse{!label.empty() ? label : "[unlabelled]"};

            throw std::runtime_error{std::format("shader_program {}: uniform \"{}\" not found", labelToUse, name)};
        }

        m_Uniforms.emplace(name, location);
        return location;
    }
}
////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CapabilityManagerFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Utilities/Casts.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"
#include "sequoia/Core/Meta/Utilities.hpp"

namespace{
    using namespace avocet::opengl;

    enum class gl_capability : GLenum {
        blend        = GL_BLEND,
        multi_sample = GL_MULTISAMPLE
    };

    enum class blend_mode : GLenum {
        zero                   = GL_ZERO,
        one                    = GL_ONE,
        src_colour             = GL_SRC_COLOR,
        one_minus_src_colour   = GL_ONE_MINUS_SRC_COLOR,
        dst_colour             = GL_DST_COLOR,
        one_minus_dst_colour   = GL_ONE_MINUS_DST_COLOR,
        src_alpha              = GL_SRC_ALPHA,
        one_minus_src_alpha    = GL_ONE_MINUS_SRC_ALPHA,
        dst_alpha              = GL_DST_ALPHA,
        one_minus_dst_alpha    = GL_ONE_MINUS_DST_ALPHA,
        const_colour           = GL_CONSTANT_COLOR,
        one_minus_const_colour = GL_ONE_MINUS_CONSTANT_COLOR,
        const_alpha            = GL_CONSTANT_ALPHA,
        one_minus_const_alpha  = GL_ONE_MINUS_CONSTANT_ALPHA
    };

    class sample_coverage_value {
        GLfloat m_Value{1.0};
    public:
        constexpr sample_coverage_value() = default;

        constexpr explicit sample_coverage_value(GLfloat val)
            : m_Value{val}
        {
            if((m_Value < 0.0) || (m_Value > 1.0))
                throw std::domain_error{std::format("Coverage value of {} requested, but must be in the range [0,1]", m_Value)};
        }

        [[nodiscard]]
        constexpr GLfloat raw_value() const noexcept { return m_Value; }

        [[nodiscard]]
        constexpr friend auto operator<=>(const sample_coverage_value&, const sample_coverage_value&) noexcept = default;
    };

    enum class invert_sample_mask : GLboolean {
        no  = GL_FALSE,
        yes = GL_TRUE
    };

    template<gl_capability Cap>
    struct capability_common_lifecycle {
        constexpr static auto capability{Cap};

        static void enable(const decorated_context& ctx) {
            gl_function{&GladGLContext::Enable}(ctx, to_gl_enum(capability));
        }

        static void disable(const decorated_context& ctx) {
            gl_function{&GladGLContext::Disable}(ctx, to_gl_enum(capability));
        }

        [[nodiscard]]
        friend constexpr bool operator==(const capability_common_lifecycle&, const capability_common_lifecycle&) noexcept = default;
    };

    namespace capabilities {
        struct gl_multi_sample : capability_common_lifecycle<gl_capability::multi_sample> {
            sample_coverage_value coverage_val{1.0};
            invert_sample_mask    mask{invert_sample_mask::no};

            void configure(this const gl_multi_sample& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::SampleCoverage}(ctx, self.coverage_val.raw_value(), static_cast<GLboolean>(self.mask));
            }

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_blend : capability_common_lifecycle<gl_capability::blend> {
            blend_mode source{blend_mode::one}, destination{blend_mode::zero};

            void configure(this const gl_blend& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::BlendFunc}(ctx, to_gl_enum(self.source), to_gl_enum(self.destination));
            }

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };
    }

    class capability_manager {
        template<class T>
        struct toggled_capability {
            T state{};
            bool is_enabled{};
        };

        using payload_type = std::tuple<toggled_capability<capabilities::gl_blend>, toggled_capability<capabilities::gl_multi_sample>>;

        payload_type m_Payload;

        const decorated_context& m_Context;
    public:
        explicit capability_manager(const decorated_context& ctx)
            : m_Context{ctx}
        {
            capabilities::gl_multi_sample::disable(m_Context);
        }

        template<class... RequestedCaps>
        void new_payload(const std::tuple<RequestedCaps...>& requestedCaps) {
            auto update{
                [&] <class Cap> (toggled_capability<Cap>& cap) {
                    constexpr auto index{sequoia::meta::find_v<std::tuple<RequestedCaps...>, Cap>};
                    if constexpr(index >= sizeof...(RequestedCaps)) {
                        if(cap.is_enabled) {
                            Cap::disable(m_Context);
                            cap.is_enabled = false;
                        }
                    }
                    else {
                        if(!cap.is_enabled) {
                            Cap::enable(m_Context);
                            cap.is_enabled = true;
                        }

                        if(const auto& requested{std::get<index>(requestedCaps)}; requested != cap.state) {
                            cap.state = requested;
                            cap.state.configure(m_Context);
                        }
                    }
                }
            };

            sequoia::meta::for_each(m_Payload, update);
        }
    };
}

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    namespace {
        [[nodiscard]]
        GLint get_int_param(const agl::decorated_context& ctx, GLenum name) {
            GLint param{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, name, &param);
            return param;
        }
    }

    [[nodiscard]]
    std::filesystem::path capability_manager_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void capability_manager_free_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
        const auto& ctx{w.context()};

        check("Multisampling enabled by default",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("Blending disabled by default",     !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capability_manager capManager{ctx};

        check("Multisampling disabled by manager", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("",                                  !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(std::tuple{capabilities::gl_blend{}});
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
        check(equality, "", get_int_param(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_ONE));
        check(equality, "", get_int_param(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ZERO));

        capManager.new_payload(std::tuple{capabilities::gl_blend{.source{blend_mode::src_alpha}, .destination{blend_mode::one_minus_src_alpha}}});
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
        check(equality, "", get_int_param(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_SRC_ALPHA));
        check(equality, "", get_int_param(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ONE_MINUS_SRC_ALPHA));

        capManager.new_payload(std::tuple{capabilities::gl_multi_sample{}});
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(std::tuple{});
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(std::tuple{capabilities::gl_multi_sample{}});
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
    }
}

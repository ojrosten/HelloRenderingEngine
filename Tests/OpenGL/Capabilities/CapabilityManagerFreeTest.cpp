////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CapabilityManagerFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Capabilities/CapabilityManager.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"
#include "sequoia/Core/Meta/Utilities.hpp"

#include <optional>
#include <tuple>

namespace avocet::opengl {

    enum class gl_capability : GLenum {
        gl_blend       = GL_BLEND,
        gl_multisample = GL_MULTISAMPLE
    };

    enum class blend_mode : GLenum {
        zero                   = GL_ZERO,
        one                    = GL_ONE,
        src_colour             = GL_SRC_COLOR,
        one_minus_src_colout   = GL_ONE_MINUS_SRC_COLOR,
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
            if((m_Value > 1.0) || (m_Value < 0.0))
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
        constexpr static gl_capability capability{Cap};

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
        struct gl_multi_sample : capability_common_lifecycle<gl_capability::gl_multisample> {
            sample_coverage_value coverage_val{1.0};
            invert_sample_mask mask{invert_sample_mask::no};

            void configure(this const gl_multi_sample& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::SampleCoverage}(
                    ctx,
                    self.coverage_val.raw_value(),
                    static_cast<GLboolean>(self.mask));
            }

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_blend : capability_common_lifecycle<gl_capability::gl_blend> {
            blend_mode source     {blend_mode::src_alpha},
                       destination{blend_mode::one_minus_src_alpha};

            void configure(this const gl_blend& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::BlendFunc}(ctx, to_gl_enum(self.source), to_gl_enum(self.destination));
            }

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };
    }

    class capability_manager {
        using payload_type = std::tuple<std::optional<capabilities::gl_blend>, std::optional<capabilities::gl_multi_sample>>;

        payload_type m_Payload{};

        const decorated_context* m_Context{};

        [[nodiscard]]
        const decorated_context& context() const { return *m_Context; }
    public:
        template<class... Capabilities>
        explicit(sizeof...(Capabilities) == 0) capability_manager(const decorated_context& ctx, const Capabilities&... caps)
            : m_Context{&ctx}
        {
            ((std::get<Capabilities>(m_Payload) = caps), ...);
 
            auto init{
                [&ctx] <class Cap> (std::optional<Cap>& optCap) {
                    if(!optCap)
                        Cap::disable(ctx);
                    else {
                        optCap->enable(ctx);
                        optCap->configure(ctx);
                    }
                }
            };

            sequoia::meta::for_each(m_Payload, init);
        }

        template<class... Capabilities>
        void new_payload(const Capabilities&... caps) {
            using incoming_tuple_t = std::tuple<const Capabilities&...>;
            auto update{
                [this, tup{incoming_tuple_t{caps...}}] <class ExistingCap> (std::optional<ExistingCap>&optCap) {
                    constexpr auto index{sequoia::meta::find_v<std::tuple<Capabilities...>, ExistingCap>};
                    if constexpr(index >= sizeof...(Capabilities)) {
                        ExistingCap::disable(context());
                    }
                    else {
                        if(const auto& requested{std::get<index>(tup)}; !optCap) {
                            optCap = requested;
                            optCap->enable(context());
                            optCap->configure(context());
                        }
                        else if(requested != optCap.value()) {
                            optCap = requested;
                            optCap->configure(context());
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
            GLint pname{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, name, &pname);
            return pname;
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

        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
        check(equality, "", get_int_param(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_ONE));
        check(equality, "", get_int_param(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ZERO));

        agl::capability_manager capManager{ctx};

        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(agl::capabilities::gl_multi_sample{});
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload();
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(agl::capabilities::gl_blend{});
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        check(equality, "", get_int_param(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_SRC_ALPHA));
        check(equality, "", get_int_param(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ONE_MINUS_SRC_ALPHA));

        capManager.new_payload(agl::capabilities::gl_blend{.source{GL_DST_ALPHA}, .destination{GL_ONE_MINUS_DST_ALPHA}});
    }
}

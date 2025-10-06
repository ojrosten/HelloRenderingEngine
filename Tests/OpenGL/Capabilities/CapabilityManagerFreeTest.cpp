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
    }
}

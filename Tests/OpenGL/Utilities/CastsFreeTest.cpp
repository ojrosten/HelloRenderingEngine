////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CastsFreeTest.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

namespace avocet::testing
{
    namespace
    {
        enum class glenum_backed    : GLenum    { zero, one };
        enum class glint_backed     : GLint     { minus_one = -1, zero, one };
        enum class glboolean_backed : GLboolean { off, on };
        enum class bool_backed      : bool      { no, yes };

        enum unscoped_glenum_backed : GLenum { unscoped_zero };

        template<class UnderlyingType, class Enum>
        inline constexpr bool has_to_gl_underlying_value_v{
            requires (Enum e) { opengl::to_gl_underlying_value<UnderlyingType>(e); }
        };
    }

    [[nodiscard]]
    std::filesystem::path casts_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void casts_free_test::run_tests()
    {
        check_invocability();
        check_values();
    }

    void casts_free_test::check_invocability()
    {
        STATIC_CHECK(has_to_gl_underlying_value_v<GLenum,    glenum_backed>);
        STATIC_CHECK(has_to_gl_underlying_value_v<GLint,     glint_backed>);
        STATIC_CHECK(has_to_gl_underlying_value_v<GLboolean, glboolean_backed>);

        STATIC_CHECK(not has_to_gl_underlying_value_v<GLint,     glenum_backed>);
        STATIC_CHECK(not has_to_gl_underlying_value_v<GLboolean, glenum_backed>);
        STATIC_CHECK(not has_to_gl_underlying_value_v<GLenum,    glint_backed>);

        STATIC_CHECK(not has_to_gl_underlying_value_v<bool,   bool_backed>);
        STATIC_CHECK(not has_to_gl_underlying_value_v<GLenum, unscoped_glenum_backed>);
    }

    void casts_free_test::check_values()
    {
        using namespace opengl;

        STATIC_CHECK(to_gl_underlying_value<GLenum>(glenum_backed::zero)      == GLenum{});
        STATIC_CHECK(to_gl_underlying_value<GLenum>(glenum_backed::one)       == GLenum{1});
        STATIC_CHECK(to_gl_underlying_value<GLint>(glint_backed::minus_one)   == GLint{-1});
        STATIC_CHECK(to_gl_underlying_value<GLint>(glint_backed::one)         == GLint{1});
        STATIC_CHECK(to_gl_underlying_value<GLboolean>(glboolean_backed::on)  == GLboolean{1});
    }
}

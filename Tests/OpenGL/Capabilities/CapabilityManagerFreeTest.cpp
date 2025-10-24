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
#include "sequoia/TestFramework/StateTransitionUtilities.hpp"
#include "sequoia/Physics/PhysicalValues.hpp"

namespace avocet::opengl {

    enum class gl_capability : GLenum {
        blend                         = GL_BLEND,
        clip_distance_0               = GL_CLIP_DISTANCE0,
        clip_distance_1               = GL_CLIP_DISTANCE1,
        clip_distance_2               = GL_CLIP_DISTANCE2,
        colour_logic_op               = GL_COLOR_LOGIC_OP,
        cull_face                     = GL_CULL_FACE,
        debug_ouptut                  = GL_DEBUG_OUTPUT,
        debug_ouptut_synchronous      = GL_DEBUG_OUTPUT_SYNCHRONOUS,
        depth_clamp                   = GL_DEPTH_CLAMP,
        depth_test                    = GL_DEPTH_TEST,
        dither                        = GL_DITHER,
        framebuffer_srgb              = GL_FRAMEBUFFER_SRGB,
        line_smooth                   = GL_LINE_SMOOTH,
        multi_sample                  = GL_MULTISAMPLE,
        polygon_offset_fill           = GL_POLYGON_OFFSET_FILL,
        polygon_offset_line           = GL_POLYGON_OFFSET_LINE,
        polygon_offset_point          = GL_POLYGON_OFFSET_POINT,
        polygon_smooth                = GL_POLYGON_SMOOTH,
        primitive_restart             = GL_PRIMITIVE_RESTART,
        primitive_restart_fixed_index = GL_PRIMITIVE_RESTART_FIXED_INDEX,
        rasterizer_discard            = GL_RASTERIZER_DISCARD,
        sample_alpha_to_coverage      = GL_SAMPLE_ALPHA_TO_COVERAGE,
        sample_alpha_to_one           = GL_SAMPLE_ALPHA_TO_ONE,
        sample_coverage               = GL_SAMPLE_COVERAGE,
        sample_shading                = GL_SAMPLE_SHADING,
        sample_mask                   = GL_SAMPLE_MASK,
        scissor_test                  = GL_SCISSOR_TEST,
        stencil_test                  = GL_STENCIL_TEST,
        texture_cube_map_seamless     = GL_TEXTURE_CUBE_MAP_SEAMLESS,
        program_point_size            = GL_PROGRAM_POINT_SIZE
    };

    std::string to_string(gl_capability cap) {
        using enum gl_capability;
        switch(cap) {
        case blend:                         return "blend";
        case clip_distance_0:               return "clip_distance_0";
        case clip_distance_1:               return "clip_distance_1";
        case clip_distance_2:               return "clip_distance_2";
        case colour_logic_op:               return "colour_logic_op";
        case cull_face:                     return "cull_face";
        case debug_ouptut:                  return "debug_ouptut";
        case debug_ouptut_synchronous:      return "debug_ouptut_synchronous";
        case depth_clamp:                   return "depth_clamp";
        case depth_test:                    return "depth_test";
        case dither:                        return "dither";
        case framebuffer_srgb:              return "framebuffer_srgb";
        case line_smooth:                   return "line_smooth";
        case multi_sample:                  return "multi_sample";
        case polygon_offset_fill:           return "polygon_offset_fill";
        case polygon_offset_line:           return "polygon_offset_line";
        case polygon_offset_point:          return "polygon_offset_point";
        case polygon_smooth:                return "polygon_smooth";
        case primitive_restart:             return "primitive_restart";
        case primitive_restart_fixed_index: return "primitive_restart_fixed_index";
        case rasterizer_discard:            return "rasterizer_discard";
        case sample_alpha_to_coverage:      return "sample_alpha_to_coverage";
        case sample_alpha_to_one:           return "sample_alpha_to_one";
        case sample_coverage:               return "sample_coverage";
        case sample_shading:                return "sample_shading";
        case sample_mask:                   return "sample_mask";
        case scissor_test:                  return "scissor_test";
        case stencil_test:                  return "stencil_test";
        case texture_cube_map_seamless:     return "texture_cube_map_seamless";
        case program_point_size:            return "program_point_size";
        }

        throw std::runtime_error{std::format("Unable to convert gl_capability::{} to a string", to_gl_enum(cap))};
    }

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

    enum class blend_eqn_mode : GLenum {
        add               = GL_FUNC_ADD,
        subtract          = GL_FUNC_SUBTRACT,
        reverse_substract = GL_FUNC_REVERSE_SUBTRACT,
        min               = GL_MIN,
        max               = GL_MAX
    };

    namespace sets {
        template<class Arena>
        struct sample_coverage_values {
            using arena_type = Arena;
        };

        template<class Arena, std::size_t N>
        struct colour_values {
            constexpr static std::size_t num_colours{N};
            using arena_type = Arena;
        };
    }

    template<std::floating_point Rep, class Arena>
    struct sample_coverage_space
      : sequoia::physics::physical_value_convex_space<sets::sample_coverage_values<Arena>, Rep, 1, sample_coverage_space<Rep, Arena>>
    {
      using arena_type           = Arena;
      using base_space           = sample_coverage_space;
      using distinguished_origin = std::true_type;
    };

    template<std::floating_point Rep, std::size_t N, class Arena>
    struct colour_space
        : sequoia::physics::physical_value_vector_space<sets::colour_values<Arena, N>, Rep, N, colour_space<Rep, N, Arena>>
    {
        using arena_type           = Arena;
        using base_space           = colour_space;
        //using distinguished_origin = std::true_type;
    };

    namespace units {
        struct rgba_t {
            using is_unit = std::true_type;
            using validator_type = std::identity;
        };

        inline constexpr rgba_t rgba{};

        struct sample_coverage_t {
            using is_unit = std::true_type;
            using validator_type = sequoia::maths::interval_validator<float, 0.0f, 1.0f>;
        };

        inline constexpr sample_coverage_t sample_coverage{};
    }
}

namespace sequoia::physics {
    template<std::floating_point T>
    struct default_space<avocet::opengl::units::sample_coverage_t, T> {
        using type = avocet::opengl::sample_coverage_space<T, implicit_common_arena>;
    };

    template<std::floating_point T>
    struct default_space<avocet::opengl::units::rgba_t, T> {
        using type = avocet::opengl::colour_space<T, 4, implicit_common_arena>;
    };
}

namespace avocet::opengl {
    using sample_coverage_value = sequoia::physics::quantity<units::sample_coverage_t, GLfloat>;
    using rgba_value            = sequoia::physics::quantity<units::rgba_t, GLfloat>;

    enum class invert_sample_mask : GLboolean {
        no  = GL_FALSE,
        yes = GL_TRUE
    };

    namespace capabilities {

        struct gl_blend_data {
            blend_mode      source{blend_mode::one},
                       destination{blend_mode::zero};
            blend_eqn_mode algebraic_op{GL_FUNC_ADD};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend_data&, const gl_blend_data&) noexcept = default;
        };

        struct gl_blend {
            constexpr static auto capability{gl_capability::blend};
            gl_blend_data rgb{}, alpha{rgb};
            rgba_value colour{};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };

        struct gl_clip_distance_0 {
            constexpr static auto capability{gl_capability::clip_distance_0};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_clip_distance_0&, const gl_clip_distance_0&) noexcept = default;
        };

        struct gl_clip_distance_1 {
            constexpr static auto capability{gl_capability::clip_distance_1};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_clip_distance_1&, const gl_clip_distance_1&) noexcept = default;
        };

        struct gl_clip_distance_2 {
            constexpr static auto capability{gl_capability::clip_distance_2};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_clip_distance_2&, const gl_clip_distance_2&) noexcept = default;
        };

        struct gl_colour_logic_op {
            constexpr static auto capability{gl_capability::colour_logic_op};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_colour_logic_op&, const gl_colour_logic_op&) noexcept = default;
        };

        struct gl_cull_face {
            constexpr static auto capability{gl_capability::cull_face};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_cull_face&, const gl_cull_face&) noexcept = default;
        };

        struct gl_debug_ouptut {
            constexpr static auto capability{gl_capability::debug_ouptut};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_debug_ouptut&, const gl_debug_ouptut&) noexcept = default;
        };

        struct gl_debug_ouptut_synchronous {
            constexpr static auto capability{gl_capability::debug_ouptut_synchronous};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_debug_ouptut_synchronous&, const gl_debug_ouptut_synchronous&) noexcept = default;
        };

        struct gl_depth_clamp {
            constexpr static auto capability{gl_capability::depth_clamp};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_depth_clamp&, const gl_depth_clamp&) noexcept = default;
        };

        struct gl_depth_test {
            constexpr static auto capability{gl_capability::depth_test};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_depth_test&, const gl_depth_test&) noexcept = default;
        };

        struct gl_dither {
            constexpr static auto capability{gl_capability::dither};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_dither&, const gl_dither&) noexcept = default;
        };

        struct gl_framebuffer_srgb {
            constexpr static auto capability{gl_capability::framebuffer_srgb};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_framebuffer_srgb&, const gl_framebuffer_srgb&) noexcept = default;
        };

        struct gl_line_smooth {
            constexpr static auto capability{gl_capability::line_smooth};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_line_smooth&, const gl_line_smooth&) noexcept = default;
        };

        struct gl_multi_sample {
            constexpr static auto capability{gl_capability::multi_sample};
            sample_coverage_value coverage_val{1.0, units::sample_coverage};
            invert_sample_mask    invert{invert_sample_mask::no};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_polygon_offset_fill {
            constexpr static auto capability{gl_capability::polygon_offset_fill};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_offset_fill&, const gl_polygon_offset_fill&) noexcept = default;
        };

        struct gl_polygon_offset_line {
            constexpr static auto capability{gl_capability::polygon_offset_line};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_offset_line&, const gl_polygon_offset_line&) noexcept = default;
        };

        struct gl_polygon_offset_point {
            constexpr static auto capability{gl_capability::polygon_offset_point};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_offset_point&, const gl_polygon_offset_point&) noexcept = default;
        };

        struct gl_polygon_smooth {
            constexpr static auto capability{gl_capability::polygon_smooth};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_smooth&, const gl_polygon_smooth&) noexcept = default;
        };

        struct gl_primitive_restart {
            constexpr static auto capability{gl_capability::primitive_restart};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_primitive_restart&, const gl_primitive_restart&) noexcept = default;
        };

        struct gl_primitive_restart_fixed_index {
            constexpr static auto capability{gl_capability::primitive_restart_fixed_index};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_primitive_restart_fixed_index&, const gl_primitive_restart_fixed_index&) noexcept = default;
        };

        struct gl_program_point_size {
            constexpr static auto capability{gl_capability::program_point_size};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_program_point_size&, const gl_program_point_size&) noexcept = default;
        };

        struct gl_rasterizer_discard {
            constexpr static auto capability{gl_capability::rasterizer_discard};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_rasterizer_discard&, const gl_rasterizer_discard&) noexcept = default;
        };

        struct gl_sample_alpha_to_coverage {
            constexpr static auto capability{gl_capability::sample_alpha_to_coverage};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_alpha_to_coverage&, const gl_sample_alpha_to_coverage&) noexcept = default;
        };

        struct gl_sample_alpha_to_one {
            constexpr static auto capability{gl_capability::sample_alpha_to_one};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_alpha_to_one&, const gl_sample_alpha_to_one&) noexcept = default;
        };

        struct gl_sample_coverage {
            constexpr static auto capability{gl_capability::sample_coverage};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_coverage&, const gl_sample_coverage&) noexcept = default;
        };

        struct gl_sample_mask {
            constexpr static auto capability{gl_capability::sample_mask};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_mask&, const gl_sample_mask&) noexcept = default;
        };

        struct gl_sample_shading {
            constexpr static auto capability{gl_capability::sample_shading};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_shading&, const gl_sample_shading&) noexcept = default;
        };

        struct gl_scissor_test {
            constexpr static auto capability{gl_capability::scissor_test};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_scissor_test&, const gl_scissor_test&) noexcept = default;
        };

        struct gl_stencil_test {
            constexpr static auto capability{gl_capability::stencil_test};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_stencil_test&, const gl_stencil_test&) noexcept = default;
        };

        struct gl_texture_cube_map_seamless {
            constexpr static auto capability{gl_capability::texture_cube_map_seamless};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_texture_cube_map_seamless&, const gl_texture_cube_map_seamless&) noexcept = default;
        };
    }

    namespace impl {
        using namespace capabilities;

        void configure(const gl_blend& previous, const gl_blend& requested, const decorated_context& ctx) {
            if(    ((requested.rgb.source   != previous.rgb.source)   || (requested.rgb.destination   != previous.rgb.destination))
                || ((requested.alpha.source != previous.alpha.source) || (requested.alpha.destination != previous.alpha.destination)))
                gl_function{&GladGLContext::BlendFuncSeparate}(ctx, to_gl_enum(requested.rgb.source), to_gl_enum(requested.rgb.destination), to_gl_enum(requested.alpha.source), to_gl_enum(requested.alpha.destination));

            if(requested.colour != previous.colour) {
                const auto& col{requested.colour.values()};
                gl_function{&GladGLContext::BlendColor}(ctx, col[0], col[1], col[2], col[3]);
            }

            if((requested.rgb.algebraic_op != previous.rgb.algebraic_op) || (requested.alpha.algebraic_op != previous.alpha.algebraic_op)) {
                gl_function{&GladGLContext::BlendEquationSeparate}(ctx, to_gl_enum(requested.rgb.algebraic_op), to_gl_enum(requested.alpha.algebraic_op));
            }
        }

        void configure(const gl_clip_distance_0& previous, const gl_clip_distance_0& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_clip_distance_1& previous, const gl_clip_distance_1& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_clip_distance_2& previous, const gl_clip_distance_2& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_colour_logic_op& previous, const gl_colour_logic_op& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_cull_face& previous, const gl_cull_face& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_debug_ouptut& previous, const gl_debug_ouptut& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_debug_ouptut_synchronous& previous, const gl_debug_ouptut_synchronous& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_depth_clamp& previous, const gl_depth_clamp& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_depth_test& previous, const gl_depth_test& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_dither& previous, const gl_dither& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_framebuffer_srgb& previous, const gl_framebuffer_srgb& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_multi_sample& previous, const gl_multi_sample& requested, const decorated_context& ctx) {
            if(requested != previous)
                gl_function{&GladGLContext::SampleCoverage}(ctx, requested.coverage_val.value(), static_cast<GLboolean>(requested.invert));
        }

        void configure(const gl_line_smooth& previous, const gl_line_smooth& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_polygon_offset_fill& previous, const gl_polygon_offset_fill& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_polygon_offset_line& previous, const gl_polygon_offset_line& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_polygon_offset_point& previous, const gl_polygon_offset_point& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_polygon_smooth& previous, const gl_polygon_smooth& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_primitive_restart& previous, const gl_primitive_restart& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_primitive_restart_fixed_index& previous, const gl_primitive_restart_fixed_index& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_rasterizer_discard& previous, const gl_rasterizer_discard& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_sample_alpha_to_coverage& previous, const gl_sample_alpha_to_coverage& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_sample_alpha_to_one& previous, const gl_sample_alpha_to_one& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_sample_coverage& previous, const gl_sample_coverage& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_sample_shading& previous, const gl_sample_shading& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_sample_mask& previous, const gl_sample_mask& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_scissor_test& previous, const gl_scissor_test& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_stencil_test& previous, const gl_stencil_test& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_texture_cube_map_seamless& previous, const gl_texture_cube_map_seamless& requested, const decorated_context& ctx)
        {
        }

        void configure(const gl_program_point_size& previous, const gl_program_point_size& requested, const decorated_context& ctx)
        {
        }

        template<class, template<class> class>
        struct rebind_class_template;

        template<class T, template<class> class To>
        using rebind_class_template_t = rebind_class_template<T, To>::type;

        template<class T, template<class> class To>
        struct rebind_class_template<std::optional<T>, To> {
            using type = To<T>;
        };

        template<class... Ts, template<class> class From, template<class> class To>
        struct rebind_class_template<std::tuple<From<Ts>...>, To> {
            using type = std::tuple<rebind_class_template_t<From<Ts>, To>...>;
        };
    }

    class capability_manager {
    public:
        using payload_type 
            = std::tuple<
                  std::optional<capabilities::gl_blend>,
                  std::optional<capabilities::gl_clip_distance_0>,
                  std::optional<capabilities::gl_clip_distance_1>,
                  std::optional<capabilities::gl_clip_distance_2>,
                  std::optional<capabilities::gl_colour_logic_op>,
                  std::optional<capabilities::gl_cull_face>,
                  std::optional<capabilities::gl_debug_ouptut>,
                  std::optional<capabilities::gl_debug_ouptut_synchronous>,
                  std::optional<capabilities::gl_depth_clamp>,
                  std::optional<capabilities::gl_depth_test>,
                  std::optional<capabilities::gl_dither>,
                  std::optional<capabilities::gl_framebuffer_srgb>,
                  std::optional<capabilities::gl_line_smooth>,
                  std::optional<capabilities::gl_multi_sample>,
                  std::optional<capabilities::gl_polygon_offset_fill>,
                  std::optional<capabilities::gl_polygon_offset_line>,
                  std::optional<capabilities::gl_polygon_offset_point>,
                  std::optional<capabilities::gl_polygon_smooth>,
                  std::optional<capabilities::gl_primitive_restart>,
                  std::optional<capabilities::gl_primitive_restart_fixed_index>,
                  std::optional<capabilities::gl_rasterizer_discard>,
                  std::optional<capabilities::gl_sample_alpha_to_coverage>,
                  std::optional<capabilities::gl_sample_alpha_to_one>,
                  std::optional<capabilities::gl_sample_coverage>,
                  std::optional<capabilities::gl_sample_shading>,
                  std::optional<capabilities::gl_sample_mask>,
                  std::optional<capabilities::gl_scissor_test>,
                  std::optional<capabilities::gl_stencil_test>,
                  std::optional<capabilities::gl_texture_cube_map_seamless>,
                  std::optional<capabilities::gl_program_point_size>
              >;

        explicit capability_manager(const decorated_context& ctx)
            : m_Context{ctx}
        {
            capability_common_lifecycle<gl_capability::dither>::disable(m_Context);
            capability_common_lifecycle<gl_capability::multi_sample>::disable(m_Context);
        }

        template<class... RequestedCaps>
        void new_payload(const std::tuple<RequestedCaps...>& requestedCaps) {
            auto updateFn{
                [&] <class Cap> (toggled_capability<Cap>& cap) {
                    constexpr auto index{sequoia::meta::find_v<std::tuple<RequestedCaps...>, Cap>};
                    if constexpr(index >= sizeof...(RequestedCaps)) {
                        disable(cap);
                    }
                    else {
                        enable(cap);
                        update(cap, std::get<index>(requestedCaps));
                    }
                }
            };

            sequoia::meta::for_each(m_Payload, updateFn);
        }

        void new_payload(const payload_type& payload) {
            [&, this] <std::size_t... Is>(std::index_sequence<Is...>) {
                (update(std::get<Is>(m_Payload), std::get<Is>(payload)), ...);
            }(std::make_index_sequence<std::tuple_size_v<payload_type>>{});
        }
    private:

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

        template<class T>
        struct toggled_capability {
            T state{};
            bool is_enabled{};

            [[nodiscard]]
            friend constexpr bool operator==(const toggled_capability&, const toggled_capability&) noexcept = default;
        };

        using toggled_payload_type = impl::rebind_class_template_t<payload_type, toggled_capability>;

        toggled_payload_type m_Payload;

        const decorated_context& m_Context;

        template<class Cap>
        void disable(toggled_capability<Cap>& cap) {
            if(cap.is_enabled) {
                capability_common_lifecycle<Cap::capability>::disable(m_Context);
                cap.is_enabled = false;
            }
        }

        template<class Cap>
        void enable(toggled_capability<Cap>& cap) {
            if(!cap.is_enabled) {
                capability_common_lifecycle<Cap::capability>::enable(m_Context);
                cap.is_enabled = true;
            }
        }

        template<class Cap>
        void update(toggled_capability<Cap>& cap, const Cap& requested) {
            impl::configure(cap.state, requested, m_Context);
            cap.state = requested;
        }

        template<class Cap>
        void update(toggled_capability<Cap>& cap, const std::optional<Cap>& requested) {
            if(!requested) {
                disable(cap);
            }
            else {
                enable(cap);
                update(cap, requested.value());
            }
        }
    };

    namespace {
        [[nodiscard]]
        GLint get_int_param(const decorated_context& ctx, GLenum name) {
            GLint param{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, name, &param);
            return param;
        }

        [[nodiscard]]
        GLboolean get_bool_param(const decorated_context& ctx, GLenum name) {
            GLboolean param{};
            gl_function{&GladGLContext::GetBooleanv}(ctx, name, &param);
            return param;
        }

        [[nodiscard]]
        GLfloat get_float_param(const decorated_context& ctx, GLenum name) {
            GLfloat param{};
            gl_function{&GladGLContext::GetFloatv}(ctx, name, &param);
            return param;
        }

        template<std::size_t N>
        [[nodiscard]]
        std::array<GLfloat, N> get_float_params(const decorated_context& ctx, GLenum name) {
            std::array<GLfloat, N> params{};
            gl_function{&GladGLContext::GetFloatv}(ctx, name, params.data());
            return params;
        }
    }
}

namespace sequoia::testing {
    using namespace avocet::opengl;

    template<>
    struct value_tester<capabilities::gl_blend_data> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const capabilities::gl_blend_data& obtained, const capabilities::gl_blend_data& predicted) {
            namespace agl = avocet::opengl;
            check(equality, "Source",       logger, agl::to_gl_enum(obtained.source),       agl::to_gl_enum(predicted.source));
            check(equality, "Destination",  logger, agl::to_gl_enum(obtained.destination),  agl::to_gl_enum(predicted.destination));
            check(equality, "Algebraic Op", logger, agl::to_gl_enum(obtained.algebraic_op), agl::to_gl_enum(predicted.algebraic_op));
        }
    };

    template<>
    struct value_tester<capabilities::gl_blend> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const capabilities::gl_blend& obtained, const capabilities::gl_blend& predicted) {
            namespace agl = avocet::opengl;
            check(equality, "RGB",    logger, obtained.rgb,    predicted.rgb);
            check(equality, "Alpha",  logger, obtained.alpha,  predicted.alpha);
            check(equality, "Colour", logger, obtained.colour, predicted.colour);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const capabilities::gl_blend& obtained, const decorated_context& ctx) {
            namespace agl = avocet::opengl;
            check(equality, "Source rgb   GPU/CPU",      logger, static_cast<GLenum>(get_int_param(ctx, GL_BLEND_SRC_RGB)),        agl::to_gl_enum(obtained.rgb.source));
            check(equality, "Source alpha GPU/CPU",      logger, static_cast<GLenum>(get_int_param(ctx, GL_BLEND_SRC_ALPHA)),      agl::to_gl_enum(obtained.alpha.source));
            check(equality, "Destination rgb   GPU/CPU", logger, static_cast<GLenum>(get_int_param(ctx, GL_BLEND_DST_RGB)),        agl::to_gl_enum(obtained.rgb.destination));
            check(equality, "Destination alpha GPU/CPU", logger, static_cast<GLenum>(get_int_param(ctx, GL_BLEND_DST_ALPHA)),      agl::to_gl_enum(obtained.alpha.destination));
            check(equality, "Blend equation GPU/CPU"   , logger, static_cast<GLenum>(get_int_param(ctx, GL_BLEND_EQUATION_RGB)),   agl::to_gl_enum(obtained.rgb.algebraic_op));
            check(equality, "Blend equation GPU/CPU"   , logger, static_cast<GLenum>(get_int_param(ctx, GL_BLEND_EQUATION_ALPHA)), agl::to_gl_enum(obtained.alpha.algebraic_op));

            check(equality, "Colour GPU/CPU",            logger, rgba_value{get_float_params<4>(ctx, GL_BLEND_COLOR), agl::units::rgba}, obtained.colour);
        }
    };

    template<>
    struct value_tester<capabilities::gl_clip_distance_0> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_clip_distance_0&, const capabilities::gl_clip_distance_0&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_clip_distance_0&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_clip_distance_1> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_clip_distance_1&, const capabilities::gl_clip_distance_1&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_clip_distance_1&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_clip_distance_2> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_clip_distance_2&, const capabilities::gl_clip_distance_2&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_clip_distance_2&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_colour_logic_op> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_colour_logic_op&, const capabilities::gl_colour_logic_op&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_colour_logic_op&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_cull_face> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_cull_face&, const capabilities::gl_cull_face&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_cull_face&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_debug_ouptut> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_debug_ouptut&, const capabilities::gl_debug_ouptut&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_debug_ouptut&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_debug_ouptut_synchronous> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_debug_ouptut_synchronous&, const capabilities::gl_debug_ouptut_synchronous&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_debug_ouptut_synchronous&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_depth_clamp> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_depth_clamp&, const capabilities::gl_depth_clamp&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_depth_clamp&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_depth_test> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_depth_test&, const capabilities::gl_depth_test&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_depth_test&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_dither> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_dither&, const capabilities::gl_dither&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_dither&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_framebuffer_srgb> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_framebuffer_srgb&, const capabilities::gl_framebuffer_srgb&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_framebuffer_srgb&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_multi_sample> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const capabilities::gl_multi_sample& obtained, const capabilities::gl_multi_sample& predicted) {
            namespace agl = avocet::opengl;
            check(equality, "Source",      logger, obtained.coverage_val,            predicted.coverage_val);
            check(equality, "Destination", logger, agl::to_gl_bool(obtained.invert), agl::to_gl_bool(predicted.invert));
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const capabilities::gl_multi_sample& obtained, const decorated_context& ctx) {
            namespace agl = avocet::opengl;
            check(equality, "Sample coverage", logger, get_float_param(ctx, GL_SAMPLE_COVERAGE_VALUE), obtained.coverage_val.value());
            check(equality, "Sample invert",   logger, get_bool_param(ctx, GL_SAMPLE_COVERAGE_INVERT), static_cast<GLboolean>(obtained.invert));
        }
    };

    template<>
    struct value_tester<capabilities::gl_line_smooth> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_line_smooth&, const capabilities::gl_line_smooth&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_line_smooth&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_polygon_offset_fill> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_polygon_offset_fill&, const capabilities::gl_polygon_offset_fill&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_polygon_offset_fill&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_polygon_offset_line> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_polygon_offset_line&, const capabilities::gl_polygon_offset_line&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_polygon_offset_line&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_polygon_offset_point> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_polygon_offset_point&, const capabilities::gl_polygon_offset_point&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_polygon_offset_point&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_polygon_smooth> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_polygon_smooth&, const capabilities::gl_polygon_smooth&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_polygon_smooth&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_primitive_restart> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_primitive_restart&, const capabilities::gl_primitive_restart&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_primitive_restart&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_primitive_restart_fixed_index> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_primitive_restart_fixed_index&, const capabilities::gl_primitive_restart_fixed_index&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_primitive_restart_fixed_index&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_program_point_size> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_program_point_size&, const capabilities::gl_program_point_size&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_program_point_size&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_rasterizer_discard> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_rasterizer_discard&, const capabilities::gl_rasterizer_discard&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_rasterizer_discard&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_sample_alpha_to_coverage> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_sample_alpha_to_coverage&, const capabilities::gl_sample_alpha_to_coverage&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_sample_alpha_to_coverage&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_sample_alpha_to_one> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_sample_alpha_to_one&, const capabilities::gl_sample_alpha_to_one&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_sample_alpha_to_one&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_sample_coverage> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_sample_coverage&, const capabilities::gl_sample_coverage&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_sample_coverage&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_sample_mask> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_sample_mask&, const capabilities::gl_sample_mask&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_sample_mask&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_sample_shading> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_sample_shading&, const capabilities::gl_sample_shading&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_sample_shading&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_scissor_test> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_scissor_test&, const capabilities::gl_scissor_test&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_scissor_test&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_stencil_test> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_stencil_test&, const capabilities::gl_stencil_test&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_stencil_test&, const decorated_context&) {
        }
    };

    template<>
    struct value_tester<capabilities::gl_texture_cube_map_seamless> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const capabilities::gl_texture_cube_map_seamless&, const capabilities::gl_texture_cube_map_seamless&) {
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const capabilities::gl_texture_cube_map_seamless&, const decorated_context&) {
        }
    };
}

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path capability_manager_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    namespace {
        using payload_type = capability_manager::payload_type;

        template<class... Caps>
        [[nodiscard]]
        payload_type make_payload(const Caps&... caps) {
            payload_type payload{};
            ((std::get<std::optional<Caps>>(payload) = std::optional<Caps>{caps}), ...);
            return payload;
        }

        template<class... Caps>
        [[nodiscard]]
        payload_type set_payload(capability_manager& manager, const capability_manager::payload_type& payload, const Caps&... caps) {
            manager.new_payload(payload);
            manager.new_payload(std::tuple{caps...});
            return make_payload(caps...);
        }
    }

    void capability_manager_free_test::run_tests()
    {
        test_default_capabilites();
    }

    void capability_manager_free_test::test_default_capabilites()
    {
        enum node_name {
            none                          = 0,
            blend                         = 1,
            clip_distance_0               = 2,
            clip_distance_1               = 3,
            clip_distance_2               = 4,
            colour_logic_op               = 5,
            cull_face                     = 6,
            debug_ouptut                  = 7,
            debug_ouptut_synchronous      = 8,
            depth_clamp                   = 9,
            depth_test                    = 10,
            dither                        = 11,
            framebuffer_srgb              = 12,
            line_smooth                   = 13,
            multi_sample                  = 14,
            polygon_offset_fill           = 15,
            polygon_offset_line           = 16,
            polygon_offset_point          = 17,
            polygon_smooth                = 18,
            primitive_restart             = 19,
            primitive_restart_fixed_index = 20,
            rasterizer_discard            = 21,
            sample_alpha_to_coverage      = 22,
            sample_alpha_to_one           = 23,
            sample_coverage               = 24,
            sample_shading                = 25,
            sample_mask                   = 26,
            scissor_test                  = 27,
            stencil_test                  = 28,
            texture_cube_map_seamless     = 29,
            program_point_size            = 30,
            blend_and_multi_sample        = 31,
            blend_set_src                 = 32,
            blend_set_dst                 = 33,
            blend_set_colour              = 34,
            blend_set_all                 = 35
        };

        using namespace curlew;
        glfw_manager manager{};
        auto w{
            manager.create_window(
                {.width {800},
                 .height{800},
                 .name{""},
                 .hiding{curlew::window_hiding_mode::on},
                 .debug_mode{agl::debugging_mode::off},
                 .prologue{},
                 .epilogue{agl::standard_error_checker{agl::num_messages{10}}},
                 .samples{1}
                }
            )
        };
        const auto& ctx{w.context()};

        check("Multisampling enabled by default",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("Blending disabled by default",     !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capability_manager capManager{ctx};

        using graph_type = transition_checker<payload_type>::transition_graph;
        using edge_type  = graph_type::edge_type;

        using namespace capabilities;

        graph_type graph{
            {
                {  // Begin Null Payload
                   {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                   },
                   {
                       node_name::blend,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}); }
                   },
                   {
                       node_name::clip_distance_0,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_clip_distance_0{}); }
                   },
                   {
                       node_name::clip_distance_1,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_clip_distance_1{}); }
                   },
                   {
                       node_name::clip_distance_2,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_clip_distance_2{}); }
                   },
                   {
                       node_name::colour_logic_op,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_colour_logic_op{}); }
                   },
                   {
                       node_name::cull_face,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_cull_face{}); }
                   },
                   {
                       node_name::debug_ouptut,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_debug_ouptut{}); }
                   },
                   {
                       node_name::debug_ouptut_synchronous,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_debug_ouptut_synchronous{}); }
                   },
                   {
                       node_name::depth_clamp,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_depth_clamp{}); }
                   },
                   {
                       node_name::depth_test,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_depth_test{}); }
                   },
                   {
                       node_name::dither,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_dither{}); }
                   },
                   {
                       node_name::framebuffer_srgb,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_framebuffer_srgb{}); }
                   },
                   {
                       node_name::line_smooth,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_line_smooth{}); }
                   },
                   {
                       node_name::multi_sample,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_multi_sample{}); }
                   },
                   {
                       node_name::polygon_offset_fill,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_polygon_offset_fill{}); }
                   },
                   {
                       node_name::polygon_offset_line,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_polygon_offset_line{}); }
                   },
                   {
                       node_name::polygon_offset_point,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_polygon_offset_point{}); }
                   },
                   {
                       node_name::polygon_smooth,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_polygon_smooth{}); }
                   },
                   {
                       node_name::primitive_restart,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_primitive_restart{}); }
                   },
                   {
                       node_name::primitive_restart_fixed_index,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_primitive_restart_fixed_index{}); }
                   },
                   {
                       node_name::rasterizer_discard,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_rasterizer_discard{}); }
                   },
                   {
                       node_name::sample_alpha_to_coverage,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_sample_alpha_to_coverage{}); }
                   },
                   {
                       node_name::sample_alpha_to_one,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_sample_alpha_to_one{}); }
                   },
                   {
                       node_name::sample_coverage,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_sample_coverage{}); }
                   },
                   {
                       node_name::sample_shading,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_sample_shading{}); }
                   },
                   {
                       node_name::sample_mask,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_sample_mask{}); }
                   },
                   {
                       node_name::scissor_test,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_scissor_test{}); }
                   },
                   {
                       node_name::stencil_test,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_stencil_test{}); }
                   },
                   {
                       node_name::texture_cube_map_seamless,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_texture_cube_map_seamless{}); }
                   },
                   {
                       node_name::blend_and_multi_sample,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}, gl_multi_sample{}); }
                   },
                   {
                       node_name::program_point_size,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_program_point_size{}); }
                   },
                   {
                       node_name::blend_set_src,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return set_payload(capManager, payload, gl_blend{.rgb{.source{blend_mode::const_alpha}, .destination{blend_mode::zero}, .algebraic_op{blend_eqn_mode::add}}});
                       }
                   },
                   {
                       node_name::blend_set_dst,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return set_payload(capManager, payload, gl_blend{.rgb{.source{blend_mode::one}, .destination{blend_mode::dst_alpha}, .algebraic_op{blend_eqn_mode::add}}});
                       }
                   },
                   {
                       node_name::blend_set_colour,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return 
                               set_payload(
                                   capManager,
                                   payload,
                                   gl_blend{
                                       .rgb{  .source{blend_mode::one}, .destination{blend_mode::zero}, .algebraic_op{blend_eqn_mode::add}},
                                       .alpha{.source{blend_mode::one}, .destination{blend_mode::zero}, .algebraic_op{blend_eqn_mode::add}},
                                       .colour{std::array{0.3f, 0.4f, 0.5f, 0.2f}, units::rgba}
                                   }
                              );
                       }
                   },
                   {
                       node_name::blend_set_all,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return 
                               set_payload(
                                   capManager,
                                   payload,
                                   gl_blend{
                                       .rgb  {.source{blend_mode::src_colour}, .destination{blend_mode::one_minus_src_colour}, .algebraic_op{blend_eqn_mode::max}},
                                       .alpha{.source{blend_mode::src_alpha},  .destination{blend_mode::one_minus_src_alpha},  .algebraic_op{blend_eqn_mode::subtract}},
                                       .colour{std::array{0.1f, 0.7f, 0.9f, 0.8f}, units::rgba}
                                   }
                              );
                       }
                   }
                }, // End Null Payload

                {  // Begin blend
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                    {
                       node_name::blend,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}); }
                    },
                    {
                       node_name::blend_set_src,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return set_payload(capManager, payload, gl_blend{.rgb{.source{blend_mode::const_alpha}, .destination{blend_mode::zero}, .algebraic_op{blend_eqn_mode::add}}});
                       }
                   },
                }, // End blend

                {  // Begin clip_distance_0
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End clip_distance_0

                {  // Begin clip_distance_1
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End clip_distance_1

                {  // Begin clip_distance_2
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End clip_distance_2

                {  // Begin colour_logic_op
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End colour_logic_op

                {  // Begin cull_face
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End cull_face

                {  // Begin debug_ouptut
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End debug_ouptut

                {  // Begin debug_ouptut_synchronous
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End debug_ouptut_synchronous

                {  // Begin depth_clamp
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End depth_clamp

                {  // Begin depth_test
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End depth_test

                {  // Begin dither
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End dither

                {  // Begin framebuffer_srgb
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End framebuffer_srgb

                {  // Begin line_smooth
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End line_smooth

                {  // Begin multi_sample
                    {
                        node_name::none,
                        "",
                        [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                    {
                       node_name::multi_sample,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_multi_sample{}); }
                    },
                }, // End multi_sample

                {  // Begin polygon_offset_fill
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End polygon_offset_fill

                {  // Begin polygon_offset_line
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End polygon_offset_line

                {  // Begin polygon_offset_point
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End polygon_offset_point

                {  // Begin polygon_smooth
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End polygon_smooth

                {  // Begin primitive_restart
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End primitive_restart

                {  // Begin primitive_restart_fixed_index
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End primitive_restart_fixed_index

                {  // Begin rasterizer_discard
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End rasterizer_discard

                {  // Begin sample_alpha_to_coverage
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End sample_alpha_to_coverage

                {  // Begin sample_alpha_to_one
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End sample_alpha_to_one

                {  // Begin sample_coverage
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End sample_coverage

                {  // Begin sample_shading
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End sample_shading

                {  // Begin sample_mask
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End sample_mask

                {  // Begin scissor_test
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End scissor_test

                {  // Begin stencil_test
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End stencil_test

                {  // Begin texture_cube_map_seamless
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End texture_cube_map_seamless

                {  // Begin program_point_size
                    {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End program_point_size

                {  // Begin blend_and_multi_sample
                    {
                        node_name::none,
                        "",
                        [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                    {
                       node_name::blend,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}); }
                    },
                    {
                       node_name::multi_sample,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_multi_sample{}); }
                    },
                    {
                       node_name::blend_and_multi_sample,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}, gl_multi_sample{}); }
                    }
                }, // End blend_and_multi_sample

                {  // Begin blend_set_src
                    {
                       node_name::blend_set_dst,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return set_payload(capManager, payload, gl_blend{.rgb{.source{blend_mode::one}, .destination{blend_mode::dst_alpha}, .algebraic_op{blend_eqn_mode::add}}});
                       }
                   },
                   {
                       node_name::blend_set_colour,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return set_payload(capManager,  payload, gl_blend{.rgb{}, .alpha{}, .colour{std::array{0.3f, 0.4f, 0.5f, 0.2f}, units::rgba}});
                       }
                   },
                   {
                       node_name::blend_set_all,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return 
                               set_payload(
                                   capManager,
                                   payload,
                                   gl_blend{
                                       .rgb  {.source{blend_mode::src_colour}, .destination{blend_mode::one_minus_src_colour},   .algebraic_op{blend_eqn_mode::max}},
                                       .alpha{.source{blend_mode::src_alpha},  .destination{blend_mode::one_minus_src_alpha}, .algebraic_op{blend_eqn_mode::subtract}},
                                       .colour{std::array{0.1f, 0.7f, 0.9f, 0.8f}, units::rgba}
                                   }
                           );
                       }
                   },
                   {
                       node_name::blend,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}); }
                   },
                   {
                       node_name::none,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                   },
                }, // End blend_set_src

                {  // Begin blend_set_dst
                    {
                       node_name::blend,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}); }
                    },
                    {
                        node_name::none,
                        "",
                        [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End blend_set_dst

                {  // Begin blend_set_colour
                    {
                       node_name::blend,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}); }
                    },
                    {
                        node_name::none,
                        "",
                        [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End blend_set_colour

                {  // Begin blend_set_all
                    {
                       node_name::blend_set_src,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type {
                           return set_payload(capManager, payload, gl_blend{.rgb{.source{blend_mode::const_alpha}, .destination{blend_mode::zero}, .algebraic_op{blend_eqn_mode::add}}});
                       }
                    },
                    {
                       node_name::blend,
                       "",
                       [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload, gl_blend{}); }
                    },
                    {
                        node_name::none,
                        "",
                        [&capManager](const payload_type& payload) -> payload_type { return set_payload(capManager, payload); }
                    },
                }, // End blend_set_all
            },
            {
                payload_type{},
                payload_type{make_payload(gl_blend{})},
                payload_type{make_payload(gl_clip_distance_0{})},
                payload_type{make_payload(gl_clip_distance_1{})},
                payload_type{make_payload(gl_clip_distance_2{})},
                payload_type{make_payload(gl_colour_logic_op{})},
                payload_type{make_payload(gl_cull_face{})},
                payload_type{make_payload(gl_debug_ouptut{})},
                payload_type{make_payload(gl_debug_ouptut_synchronous{})},
                payload_type{make_payload(gl_depth_clamp{})},
                payload_type{make_payload(gl_depth_test{})},
                payload_type{make_payload(gl_dither{})},
                payload_type{make_payload(gl_framebuffer_srgb{})},
                payload_type{make_payload(gl_line_smooth{})},
                payload_type{make_payload(gl_multi_sample{})},
                payload_type{make_payload(gl_polygon_offset_fill{})},
                payload_type{make_payload(gl_polygon_offset_line{})},
                payload_type{make_payload(gl_polygon_offset_point{})},
                payload_type{make_payload(gl_polygon_smooth{})},
                payload_type{make_payload(gl_primitive_restart{})},
                payload_type{make_payload(gl_primitive_restart_fixed_index{})},
                payload_type{make_payload(gl_rasterizer_discard{})},
                payload_type{make_payload(gl_sample_alpha_to_coverage{})},
                payload_type{make_payload(gl_sample_alpha_to_one{})},
                payload_type{make_payload(gl_sample_coverage{})},
                payload_type{make_payload(gl_sample_shading{})},
                payload_type{make_payload(gl_sample_mask{})},
                payload_type{make_payload(gl_scissor_test{})},
                payload_type{make_payload(gl_stencil_test{})},
                payload_type{make_payload(gl_texture_cube_map_seamless{})},
                payload_type{make_payload(gl_program_point_size{})},
                payload_type{make_payload(gl_blend{}, gl_multi_sample{})},
                payload_type{make_payload(gl_blend{.rgb{.source{blend_mode::const_alpha}, .destination{blend_mode::zero},      .algebraic_op{blend_eqn_mode::add}}})},
                payload_type{make_payload(gl_blend{.rgb{.source{blend_mode::one},         .destination{blend_mode::dst_alpha}, .algebraic_op{blend_eqn_mode::add}}})},
                payload_type{make_payload(gl_blend{.rgb{}, .alpha{}, .colour{std::array{0.3f, 0.4f, 0.5f, 0.2f}, units::rgba}})},
                payload_type{
                    make_payload(
                        gl_blend{
                            .rgb  {.source{blend_mode::src_colour}, .destination{blend_mode::one_minus_src_colour}, .algebraic_op{blend_eqn_mode::max}},
                            .alpha{.source{blend_mode::src_alpha},  .destination{blend_mode::one_minus_src_alpha},  .algebraic_op{blend_eqn_mode::subtract}},
                            .colour{std::array{0.1f, 0.7f, 0.9f, 0.8f}, units::rgba}
                        }
                    )
                },
            }
        };

        auto checker{
            [&ctx, this](std::string_view description, const payload_type& obtained, const payload_type& predicted) {
                check(equality, {description, no_source_location}, obtained, predicted);

               auto checkGPU{
                   [&] <class Cap>(const std::optional<Cap>& cap) {
                       check(equality, "Is enabled", static_cast<bool>(agl::gl_function{&GladGLContext::IsEnabled}(ctx, to_gl_enum(Cap::capability))), static_cast<bool>(cap));
                       if(cap)
                          check(weak_equivalence, "GPU State", cap.value(), ctx);
                   }
               };

               sequoia::meta::for_each(obtained, checkGPU);
            }
        };

        transition_checker<payload_type>::check("", graph, checker);
    }
}

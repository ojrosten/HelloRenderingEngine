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

    namespace sets {
        template<class Arena>
        struct coverage_values {
            using arena_type = Arena;
        };
    }

    template<std::floating_point Rep, class Arena>
    struct sample_coverage_space
      : sequoia::physics::physical_value_convex_space<sets::coverage_values<Arena>, Rep, 1, sample_coverage_space<Rep, Arena>>
    {
      using arena_type           = Arena;
      using base_space           = sample_coverage_space;
      using distinguished_origin = std::true_type;
    };

    struct sample_coverage_t {
        using is_unit        = std::true_type;
        using validator_type = sequoia::maths::interval_validator<float, 0.0f, 1.0f>;
    };

    inline constexpr sample_coverage_t sample_coverage{};
}

namespace sequoia::physics {
    template<std::floating_point T>
    struct default_space<sample_coverage_t, T>
    {
        using type = sample_coverage_space<T, implicit_common_arena>;
    };
}

namespace {
    using sample_coverage_value = sequoia::physics::quantity<sample_coverage_t, float>;

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
        struct gl_multi_sample {
            constexpr static auto capability{gl_capability::multi_sample};
            sample_coverage_value coverage_val{1.0, sample_coverage};
            invert_sample_mask    mask{invert_sample_mask::no};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_blend {
            constexpr static auto capability{gl_capability::blend};
            blend_mode source{blend_mode::one}, destination{blend_mode::zero};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };
    }

    namespace impl {
        void configure(const capabilities::gl_multi_sample& cap, const decorated_context& ctx) {
            gl_function{&GladGLContext::SampleCoverage}(ctx, cap.coverage_val.value(), static_cast<GLboolean>(cap.mask));
        }


        void configure(const capabilities::gl_blend& cap, const decorated_context& ctx) {
            gl_function{&GladGLContext::BlendFunc}(ctx, to_gl_enum(cap.source), to_gl_enum(cap.destination));
        }
    }

    template<class T>
    struct toggled_capability {
        T state{};
        bool is_enabled{};

        [[nodiscard]]
        friend constexpr bool operator==(const toggled_capability&, const toggled_capability&) noexcept = default;
    };

    class capability_manager {
    public:
        using payload_type = std::tuple<toggled_capability<capabilities::gl_blend>, toggled_capability<capabilities::gl_multi_sample>>;

        explicit capability_manager(const decorated_context& ctx)
            : m_Context{ctx}
        {
            capability_common_lifecycle<gl_capability::multi_sample>::disable(m_Context);
        }

        template<class... RequestedCaps>
        const payload_type& new_payload(const std::tuple<RequestedCaps...>& requestedCaps) {
            auto update{
                [&] <class Cap> (toggled_capability<Cap>& cap) {
                    constexpr auto index{sequoia::meta::find_v<std::tuple<RequestedCaps...>, Cap>};
                    if constexpr(index >= sizeof...(RequestedCaps)) {
                        if(cap.is_enabled) {
                            capability_common_lifecycle<Cap::capability>::disable(m_Context);
                            cap.is_enabled = false;
                        }
                    }
                    else {
                        if(!cap.is_enabled) {
                            capability_common_lifecycle<Cap::capability>::enable(m_Context);
                            cap.is_enabled = true;
                        }

                        if(const auto& requested{std::get<index>(requestedCaps)}; requested != cap.state) {
                            cap.state = requested;
                            impl::configure(cap.state, m_Context);
                        }
                    }
                }
            };

            sequoia::meta::for_each(m_Payload, update);

            return m_Payload;
        }

        [[nodiscard]]
        const payload_type& payload() const noexcept { return m_Payload; }

    private:
        payload_type m_Payload;

        const decorated_context& m_Context;
    };
}

namespace sequoia::testing {

    template<class T>
    struct value_tester<toggled_capability<T>> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const toggled_capability<T>& obtained, const toggled_capability<T>& prediction)
        {
            namespace agl = avocet::opengl;
            check(equality, "State",      logger, obtained.state,      prediction.state);
            check(equality, "Is Enabled", logger, obtained.is_enabled, prediction.is_enabled);
        }
    };

    template<>
    struct value_tester<capabilities::gl_blend> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const capabilities::gl_blend& obtained, const capabilities::gl_blend& prediction)
        {
            namespace agl = avocet::opengl;
            check(equality, "Source",      logger, agl::to_gl_enum(obtained.source),      agl::to_gl_enum(prediction.source));
            check(equality, "Destination", logger, agl::to_gl_enum(obtained.destination), agl::to_gl_enum(prediction.destination));
        }
    };

    template<>
    struct value_tester<capabilities::gl_multi_sample> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const capabilities::gl_multi_sample& obtained, const capabilities::gl_multi_sample& prediction)
        {
            namespace agl = avocet::opengl;
            check(equality, "Source",      logger, obtained.coverage_val,          prediction.coverage_val);
            check(equality, "Destination", logger, agl::to_gl_bool(obtained.mask), agl::to_gl_bool(prediction.mask));
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

    namespace {
        enum node_name {
            none         = 0,
            blend        = 1,
            multi_sample = 2,
        };

        /*template<gl_capability Cap>
        [[nodiscard]]
        capabilities_type to_capabilities() {

        }*/
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
        using payload_type = capability_manager::payload_type;

        using graph_type = transition_checker<payload_type>::transition_graph;
        using edge_type  = graph_type::edge_type;

        using namespace capabilities;

        graph_type graph{
            {
                {  // Begin Null Payload
                   {
                       node_name::none,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{}); }
                   },
                   {
                       node_name::blend,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_blend{}}); }
                   },
                   {
                       node_name::multi_sample,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_multi_sample{}}); }
                   },
                   {
                       node_name::blend | node_name::multi_sample,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_blend{}, gl_multi_sample{}}); }
                   },
                   {
                       node_name::blend | node_name::multi_sample,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_multi_sample{}, gl_blend{}}); }
                   }
                }, // End Null Payload
                {  // Begin blend
                    {
                        node_name::none,
                        "",
                        [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{}); }
                    },
                    {
                       node_name::blend,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_blend{}}); }
                    },
                }, //   End blend
                {  // Begin multi_sample
                    {
                        node_name::none,
                        "",
                        [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{}); }
                    },
                    {
                       node_name::multi_sample,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_multi_sample{}}); }
                    },
                }, //   End multi_sample
                {  // Begin blend | multi_sample
                    {
                        node_name::none,
                        "",
                        [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{}); }
                    },
                    {
                       node_name::blend,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_blend{}}); }
                    },
                    {
                       node_name::multi_sample,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_multi_sample{}}); }
                    },
                    {
                       node_name::blend | node_name::multi_sample,
                       "",
                       [&capManager](payload_type) -> payload_type { return capManager.new_payload(std::tuple{gl_blend{}, gl_multi_sample{}}); }
                    }
                }, //   End blend | multi_sample
            },
            {
                payload_type{},
                payload_type{toggled_capability{gl_blend{}, true},  toggled_capability{gl_multi_sample{},  false}},
                payload_type{toggled_capability{gl_blend{}, false}, toggled_capability{gl_multi_sample{}, true}},
                payload_type{toggled_capability{gl_blend{}, true},  toggled_capability{gl_multi_sample{},  true}},
            }
        };

        auto checker{
            [&ctx, this](std::string_view description, const payload_type& obtained, const payload_type& prediction) {
                check(equality, {description, no_source_location}, obtained, prediction);

                auto checkGPU{
                    [&] <class Cap>(const toggled_capability<Cap>& cap) {
                        check(equality, "GPU State", cap.is_enabled, static_cast<bool>(agl::gl_function{&GladGLContext::IsEnabled}(ctx, agl::to_gl_enum(Cap::capability))));
                    }
                };

                sequoia::meta::for_each(obtained, checkGPU);
            }
        };

        transition_checker<payload_type>::check("", graph, checker);


        /*check("Multisampling disabled by manager", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
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
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));*/
    }
}

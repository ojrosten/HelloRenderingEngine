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

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"

#include <optional>
#include <tuple>

namespace avocet::opengl {
    template<GLenum Cap>
    struct gl_capability {
        constexpr static GLenum raw_capability{Cap};

        static void enable(const decorated_context& ctx) {
            gl_function{&GladGLContext::Enable}(ctx, raw_capability);
        }

        static void disable(const decorated_context& ctx) {
            gl_function{&GladGLContext::Disable}(ctx, raw_capability);
        }

        [[nodiscard]]
        friend constexpr bool operator==(const gl_capability&, const gl_capability&) noexcept = default;
    };

    namespace capabilities {
        struct gl_multi_sample : gl_capability<GL_MULTISAMPLE> {

            void configure(const decorated_context&) const {}

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_blend : gl_capability<GL_BLEND> {
            GLenum source{GL_SRC_ALPHA}, destination{GL_ONE_MINUS_SRC_ALPHA};

            void configure(this const gl_blend& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::BlendFunc}(ctx, self.source, self.destination);
            }

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };
    }

    class capability_manager {
        using tuple_t = std::tuple<std::optional<capabilities::gl_blend>, std::optional<capabilities::gl_multi_sample>>;
        tuple_t m_Payload{};

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

            [init, this] <std::size_t... Is>(std::index_sequence<Is...>) {
                (init(std::get<Is>(m_Payload)), ...);
            }(std::make_index_sequence<std::tuple_size_v<tuple_t>>{});
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
                        const auto& requested{std::get<index>(tup)};
                        if(!optCap) {
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

            [update, this] <std::size_t... Is>(std::index_sequence<Is...>) {
                (update(std::get<Is>(m_Payload)), ...);
            }(std::make_index_sequence<std::tuple_size_v<tuple_t>>{});
        }
    };
}

namespace avocet::testing
{
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

        namespace agl = avocet::opengl;
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

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
    }
}

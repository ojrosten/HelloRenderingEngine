////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CapabilityManagerFreeTest.hpp"
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
    };

    namespace capabilities {
        struct gl_multi_sample : gl_capability<GL_MULTISAMPLE> {

            void configure(const decorated_context&) const {}

            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_blend : gl_capability<GL_BLEND> {
            GLenum source{GL_SRC_ALPHA}, destination{GL_ONE_MINUS_SRC_ALPHA};

            void configure(this const gl_blend& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::BlendFunc}(ctx, self.source, self.destination);
            }

            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };
    }

    class capability_manager {
        std::tuple<std::optional<capabilities::gl_blend>, std::optional<capabilities::gl_multi_sample>> m_Payload{};

        const decorated_context* m_Context{};
    public:
        template<class... Capabilities>
        explicit(sizeof...(Capabilities) == 0) capability_manager(const decorated_context& ctx, const Capabilities&... caps)
            : m_Context{ctx}
        {
            ((std::get<Capabilities>(m_Payload) = caps), ...);
 
            auto init{
                [&ctx] <class Cap> (std::optional<Cap>& optCap) {
                    if(!optCap)
                        Cap::disable(ctx);
                    else {
                        optCap->enable();
                        optCap->configure();
                    }
                }
            };

            std::apply(init, m_Payload);
        }

        template<class... Capabilities>
        void reset_payload(const Capabilities&... caps) {
            auto update{
                [] <class ExistingCap> (std::optional<ExistingCap>& optCap) {
                    constexpr auto index{sequoia::meta::find_v<std::tuple<Capabilities...>, ExistingCap>};
                    if(index >= sizeof(Capabilities...)) {
                        ExistingCap::disable(m_Context);
                    }
                    else {
                        std::tuple<const Capabilities&...> tup{caps...};
                        const auto& requested{std::get<index>(tup)};
                        if(!optCap) {
                            optCap = requested;
                            optCap->enable();
                            optCap->configure();
                        }
                        else if(requested != optCap.value()) {
                            optCap = requested;
                            optCap->configure();
                        }
                    }
                }
            };

            std::apply(update, m_Payload);
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
    }
}

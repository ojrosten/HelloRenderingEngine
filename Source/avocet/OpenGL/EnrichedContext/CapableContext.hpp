////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Capabilities/CapabilitiesConfiguration.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"
#include "sequoia/Core/Meta/Utilities.hpp"

namespace avocet::opengl {
    enum class attempt_to_compensate_for_driver_bugs : bool { no, yes };

    class capable_context : public decorated_context {
        template<class T>
        struct toggled_capability {
            T state{};
            bool is_enabled{(T::capability == gl_capability::dither) or (T::capability == gl_capability::multi_sample)};
        };

        using toggled_payload_type
            = std::tuple<
                  toggled_capability<capabilities::gl_blend>,
                  toggled_capability<capabilities::gl_depth_test>,
                  toggled_capability<capabilities::gl_multi_sample>,
                  toggled_capability<capabilities::gl_polygon_offset_fill>,
                  toggled_capability<capabilities::gl_polygon_offset_line>,
                  toggled_capability<capabilities::gl_polygon_offset_point>,
                  toggled_capability<capabilities::gl_sample_alpha_to_coverage>,
                  toggled_capability<capabilities::gl_sample_coverage>,
                  toggled_capability<capabilities::gl_stencil_test>
              >;

        mutable toggled_payload_type m_Payload{};

        template<class Cap>
        void disable(this const capable_context& self, toggled_capability<Cap>& cap) {
            if(cap.is_enabled) {
                gl_function{&GladGLContext::Disable}(self, to_gl_enum(Cap::capability));
                cap.is_enabled = false;
            }
        }

        template<class Cap>
        void enable(this const capable_context& self, toggled_capability<Cap>& cap) {
            if(!cap.is_enabled) {
                gl_function{&GladGLContext::Enable}(self, to_gl_enum(Cap::capability));
                cap.is_enabled = true;
            }
        }

        template<class Cap>
        void update_config(this const capable_context& self, toggled_capability<Cap>& current, const Cap& requested) {
            if(current.state != requested) {
                capabilities::impl::configure(self, current.state, requested);
                current.state = requested;
            }
        }

        template<class Cap>
        void full_update(this const capable_context& self, toggled_capability<Cap>& current, const std::optional<Cap>& requested) {
            if(!requested) {
                self.disable(current);
            }
            else {
                self.enable(current);
                self.update_config(current, requested.value());
            }
        }
    public:
        using payload_type
            = std::tuple<
                  std::optional<capabilities::gl_blend>,
                  std::optional<capabilities::gl_depth_test>,
                  std::optional<capabilities::gl_multi_sample>,
                  std::optional<capabilities::gl_polygon_offset_fill>,
                  std::optional<capabilities::gl_polygon_offset_line>,
                  std::optional<capabilities::gl_polygon_offset_point>,
                  std::optional<capabilities::gl_sample_alpha_to_coverage>,
                  std::optional<capabilities::gl_sample_coverage>,
                  std::optional<capabilities::gl_stencil_test>
              >;

        template<class Fn>
        constexpr static bool is_decorator_v{std::is_invocable_r_v<void, Fn, const context&, const decorator_data&>};

        template<class Loader, class Prologue, class Epilogue>
            requires is_decorator_v<Prologue>
                  && is_decorator_v<Epilogue>
                  && std::is_invocable_r_v<GladGLContext, Loader, GladGLContext>
        capable_context(debugging_mode mode, Loader loader, Prologue prologue, Epilogue epilogue, attempt_to_compensate_for_driver_bugs compensate)
            : decorated_context{mode, std::move(loader), std::move(prologue), std::move(epilogue)}
        {
             sequoia::meta::for_each(m_Payload, [this](auto& cap) { disable(cap); });
             if(compensate == attempt_to_compensate_for_driver_bugs::yes)
                 capabilities::impl::compensate_for_driver_init_bugs(*this, capabilities::gl_stencil_test{});
        }

        void new_payload(this const capable_context& self, const payload_type& payload) {
            [&] <std::size_t... Is>(std::index_sequence<Is...>) {
                (self.full_update(std::get<Is>(self.m_Payload), std::get<Is>(payload)), ...);
            }(std::make_index_sequence<std::tuple_size_v<toggled_payload_type>>{});
        }
    };
}

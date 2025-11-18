////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Capabilities/Capabilities.hpp"

#include "avocet/OpenGL/Utilities/Casts.hpp"

namespace avocet::opengl {

    [[nodiscard]]
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

        throw std::runtime_error{std::format("Unrecognized option gl_capability::{} while stringifying", to_gl_enum(cap))};
    }

    [[nodiscard]]
    std::string to_string(blend_mode mode) {
        using enum blend_mode;
        switch(mode)
        {
        case zero:                   return "zero";
        case one:                    return "one";
        case src_colour:             return "src_colour";
        case one_minus_src_colour:   return "one_minus_src_colour";
        case dst_colour:             return "dst_colour";
        case one_minus_dst_colour:   return "one_minus_dst_colour";
        case src_alpha:              return "src_alpha";
        case one_minus_src_alpha:    return "one_minus_src_alpha";
        case dst_alpha:              return "dst_alpha";
        case one_minus_dst_alpha:    return "one_minus_dst_alpha";
        case const_colour:           return "const_colour";
        case one_minus_const_colour: return "one_minus_const_colour";
        case const_alpha:            return "const_alpha";
        case one_minus_const_alpha:  return "one_minus_const_alpha ";
        }

        throw std::runtime_error{std::format("Unrecognized option blend_mode::{} while stringifying", to_gl_enum(mode))};
    }

    [[nodiscard]]
    std::string to_string(blend_eqn_mode mode) {
        using enum blend_eqn_mode;
        switch(mode)
        {
        case add:              return "add";
        case subtract:         return "subtract";
        case reverse_subtract: return "reverse_subtract";
        case min:              return "min";
        case max:              return "max";
        }

        throw std::runtime_error{std::format("Unrecognized option blend_eqn_mode::{} while stringifying", to_gl_enum(mode))};
    }
}
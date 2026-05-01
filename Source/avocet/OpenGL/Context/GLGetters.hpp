////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

namespace avocet::opengl {
    enum class int_names {
        active_texture                                  = GL_ACTIVE_TEXTURE,
        array_buffer_binding                            = GL_ARRAY_BUFFER_BINDING,
        blend_dst_alpha                                 = GL_BLEND_DST_ALPHA,
        blend_dst_rgb                                   = GL_BLEND_DST_RGB,
        blend_equation_alpha                            = GL_BLEND_EQUATION_ALPHA,
        blend_equation_rgb                              = GL_BLEND_EQUATION_RGB,
        blend_src_alpha                                 = GL_BLEND_SRC_ALPHA,
        blend_src_rgb                                   = GL_BLEND_SRC_RGB,
        context_flags                                   = GL_CONTEXT_FLAGS,
        copy_read_buffer_binding                        = GL_COPY_READ_BUFFER_BINDING,
        copy_write_buffer_binding                       = GL_COPY_WRITE_BUFFER_BINDING,
        cull_face_mode                                  = GL_CULL_FACE_MODE,
        current_program                                 = GL_CURRENT_PROGRAM,
        debug_group_stack_depth                         = GL_DEBUG_GROUP_STACK_DEPTH,
        depth_func                                      = GL_DEPTH_FUNC,
        draw_buffer                                     = GL_DRAW_BUFFER,
        draw_framebuffer_binding                        = GL_DRAW_FRAMEBUFFER_BINDING,
        element_array_buffer_binding                    = GL_ELEMENT_ARRAY_BUFFER_BINDING,
        fragment_shader_derivative_hint                 = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
        front_face                                      = GL_FRONT_FACE,
        line_smooth_hint                                = GL_LINE_SMOOTH_HINT,
        logic_op_mode                                   = GL_LOGIC_OP_MODE,
        major_version                                   = GL_MAJOR_VERSION,
        max_3d_texture_size                             = GL_MAX_3D_TEXTURE_SIZE,
        max_array_texture_layers                        = GL_MAX_ARRAY_TEXTURE_LAYERS,
        max_atomic_counter_buffer_bindings              = GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS,
        max_color_attachments                           = GL_MAX_COLOR_ATTACHMENTS,
        max_color_texture_samples                       = GL_MAX_COLOR_TEXTURE_SAMPLES,
        max_combined_atomic_counters                    = GL_MAX_COMBINED_ATOMIC_COUNTERS,
        max_combined_compute_uniform_components         = GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,
        max_combined_fragment_uniform_components        = GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
        max_combined_geometry_uniform_components        = GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,
        max_combined_shader_storage_blocks              = GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,
        max_combined_tess_control_uniform_components    = GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS,
        max_combined_tess_evaluation_uniform_components = GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS,
        max_combined_texture_image_units                = GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
        max_combined_uniform_blocks                     = GL_MAX_COMBINED_UNIFORM_BLOCKS,
        max_combined_vertex_uniform_components          = GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
        max_compute_atomic_counters                     = GL_MAX_COMPUTE_ATOMIC_COUNTERS,
        max_compute_atomic_counter_buffers              = GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,
        max_compute_shader_storage_blocks               = GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,
        max_compute_texture_image_units                 = GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,
        max_compute_uniform_blocks                      = GL_MAX_COMPUTE_UNIFORM_BLOCKS,
        max_compute_uniform_components                  = GL_MAX_COMPUTE_UNIFORM_COMPONENTS,
        max_compute_work_group_invocations              = GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,
        max_cube_map_texture_size                       = GL_MAX_CUBE_MAP_TEXTURE_SIZE,
        max_debug_group_stack_depth                     = GL_MAX_DEBUG_GROUP_STACK_DEPTH,
        max_debug_logged_messages                       = GL_MAX_DEBUG_LOGGED_MESSAGES,
        max_debug_message_length                        = GL_MAX_DEBUG_MESSAGE_LENGTH,
        max_depth_texture_samples                       = GL_MAX_DEPTH_TEXTURE_SAMPLES,
        max_draw_buffers                                = GL_MAX_DRAW_BUFFERS,
        max_dual_source_draw_buffers                    = GL_MAX_DUAL_SOURCE_DRAW_BUFFERS,
        max_element_index                               = GL_MAX_ELEMENT_INDEX,
        max_elements_indices                            = GL_MAX_ELEMENTS_INDICES,
        max_elements_vertices                           = GL_MAX_ELEMENTS_VERTICES,
        max_fragment_atomic_counters                    = GL_MAX_FRAGMENT_ATOMIC_COUNTERS,
        max_fragment_input_components                   = GL_MAX_FRAGMENT_INPUT_COMPONENTS,
        max_fragment_shader_storage_blocks              = GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
        max_fragment_uniform_blocks                     = GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
        max_fragment_uniform_components                 = GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
        max_fragment_uniform_vectors                    = GL_MAX_FRAGMENT_UNIFORM_VECTORS,
        max_framebuffer_height                          = GL_MAX_FRAMEBUFFER_HEIGHT,
        max_framebuffer_layers                          = GL_MAX_FRAMEBUFFER_LAYERS,
        max_framebuffer_samples                         = GL_MAX_FRAMEBUFFER_SAMPLES,
        max_framebuffer_width                           = GL_MAX_FRAMEBUFFER_WIDTH,
        max_geometry_atomic_counters                    = GL_MAX_GEOMETRY_ATOMIC_COUNTERS,
        max_geometry_input_components                   = GL_MAX_GEOMETRY_INPUT_COMPONENTS,
        max_geometry_output_components                  = GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,
        max_geometry_output_vertices                    = GL_MAX_GEOMETRY_OUTPUT_VERTICES,
        max_geometry_shader_storage_blocks              = GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
        max_geometry_texture_image_units                = GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,
        max_geometry_uniform_blocks                     = GL_MAX_GEOMETRY_UNIFORM_BLOCKS,
        max_geometry_uniform_components                 = GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
        max_integer_samples                             = GL_MAX_INTEGER_SAMPLES,
        max_label_length                                = GL_MAX_LABEL_LENGTH,
        max_program_texel_offset                        = GL_MAX_PROGRAM_TEXEL_OFFSET,
        max_rectangle_texture_size                      = GL_MAX_RECTANGLE_TEXTURE_SIZE,
        max_renderbuffer_size                           = GL_MAX_RENDERBUFFER_SIZE,
        max_sample_mask_words                           = GL_MAX_SAMPLE_MASK_WORDS,
        max_shader_storage_buffer_bindings              = GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
        max_tess_control_atomic_counters                = GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,
        max_tess_control_input_components               = GL_MAX_TESS_CONTROL_INPUT_COMPONENTS,
        max_tess_control_output_components              = GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS,
        max_tess_control_shader_storage_blocks          = GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
        max_tess_control_texture_image_units            = GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS,
        max_tess_control_uniform_blocks                 = GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,
        max_tess_control_uniform_components             = GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS,
        max_tess_evaluation_atomic_counters             = GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,
        max_tess_evaluation_input_components            = GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS,
        max_tess_evaluation_output_components           = GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS,
        max_tess_evaluation_shader_storage_blocks       = GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
        max_tess_evaluation_texture_image_units         = GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS,
        max_tess_evaluation_uniform_blocks              = GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS,
        max_tess_evaluation_uniform_components          = GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS,
        max_tess_gen_level                              = GL_MAX_TESS_GEN_LEVEL,
        max_tess_patch_components                       = GL_MAX_TESS_PATCH_COMPONENTS,
        max_texture_buffer_size                         = GL_MAX_TEXTURE_BUFFER_SIZE,
        max_texture_image_units                         = GL_MAX_TEXTURE_IMAGE_UNITS,
        max_texture_size                                = GL_MAX_TEXTURE_SIZE,
        max_uniform_block_size                          = GL_MAX_UNIFORM_BLOCK_SIZE,
        max_uniform_buffer_bindings                     = GL_MAX_UNIFORM_BUFFER_BINDINGS,
        max_uniform_locations                           = GL_MAX_UNIFORM_LOCATIONS,
        max_varying_components                          = GL_MAX_VARYING_COMPONENTS,
        max_varying_floats                              = GL_MAX_VARYING_FLOATS,
        max_varying_vectors                             = GL_MAX_VARYING_VECTORS,
        max_vertex_atomic_counters                      = GL_MAX_VERTEX_ATOMIC_COUNTERS,
        max_vertex_attrib_bindings                      = GL_MAX_VERTEX_ATTRIB_BINDINGS,
        max_vertex_attrib_relative_offset               = GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,
        max_vertex_attribs                              = GL_MAX_VERTEX_ATTRIBS,
        max_vertex_output_components                    = GL_MAX_VERTEX_OUTPUT_COMPONENTS,
        max_vertex_shader_storage_blocks                = GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,
        max_vertex_texture_image_units                  = GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        max_vertex_uniform_blocks                       = GL_MAX_VERTEX_UNIFORM_BLOCKS,
        max_vertex_uniform_components                   = GL_MAX_VERTEX_UNIFORM_COMPONENTS,
        max_vertex_uniform_vectors                      = GL_MAX_VERTEX_UNIFORM_VECTORS,
        min_map_buffer_alignment                        = GL_MIN_MAP_BUFFER_ALIGNMENT,
        min_program_texel_offset                        = GL_MIN_PROGRAM_TEXEL_OFFSET,
        minor_version                                   = GL_MINOR_VERSION,
        num_compressed_texture_formats                  = GL_NUM_COMPRESSED_TEXTURE_FORMATS,
        num_extensions                                  = GL_NUM_EXTENSIONS,
        num_program_binary_formats                      = GL_NUM_PROGRAM_BINARY_FORMATS,
        num_shader_binary_formats                       = GL_NUM_SHADER_BINARY_FORMATS,
        pack_alignment                                  = GL_PACK_ALIGNMENT,
        pack_image_height                               = GL_PACK_IMAGE_HEIGHT,
        pack_row_length                                 = GL_PACK_ROW_LENGTH,
        pack_skip_images                                = GL_PACK_SKIP_IMAGES,
        pack_skip_pixels                                = GL_PACK_SKIP_PIXELS,
        pack_skip_rows                                  = GL_PACK_SKIP_ROWS,
        pixel_pack_buffer_binding                       = GL_PIXEL_PACK_BUFFER_BINDING,
        pixel_unpack_buffer_binding                     = GL_PIXEL_UNPACK_BUFFER_BINDING,
        polygon_mode                                    = GL_POLYGON_MODE,
        polygon_smooth_hint                             = GL_POLYGON_SMOOTH_HINT,
        primitive_restart_index                         = GL_PRIMITIVE_RESTART_INDEX,
        program_binary_formats                          = GL_PROGRAM_BINARY_FORMATS,
        program_pipeline_binding                        = GL_PROGRAM_PIPELINE_BINDING,
        provoking_vertex                                = GL_PROVOKING_VERTEX,
        read_buffer                                     = GL_READ_BUFFER,
        read_framebuffer_binding                        = GL_READ_FRAMEBUFFER_BINDING,
        renderbuffer_binding                            = GL_RENDERBUFFER_BINDING,
        sample_mask_value                               = GL_SAMPLE_MASK_VALUE,
        sample_buffers                                  = GL_SAMPLE_BUFFERS,
        samples                                         = GL_SAMPLES,
        shader_storage_buffer_binding                   = GL_SHADER_STORAGE_BUFFER_BINDING,
        shader_storage_buffer_offset_alignment          = GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,
        stencil_back_fail                               = GL_STENCIL_BACK_FAIL,
        stencil_back_func                               = GL_STENCIL_BACK_FUNC,
        stencil_back_pass_depth_fail                    = GL_STENCIL_BACK_PASS_DEPTH_FAIL,
        stencil_back_pass_depth_pass                    = GL_STENCIL_BACK_PASS_DEPTH_PASS,
        stencil_back_ref                                = GL_STENCIL_BACK_REF,
        stencil_back_value_mask                         = GL_STENCIL_BACK_VALUE_MASK,
        stencil_back_writemask                          = GL_STENCIL_BACK_WRITEMASK,
        stencil_clear_value                             = GL_STENCIL_CLEAR_VALUE,
        stencil_fail                                    = GL_STENCIL_FAIL,
        stencil_func                                    = GL_STENCIL_FUNC,
        stencil_pass_depth_fail                         = GL_STENCIL_PASS_DEPTH_FAIL,
        stencil_pass_depth_pass                         = GL_STENCIL_PASS_DEPTH_PASS,
        stencil_ref                                     = GL_STENCIL_REF,
        stencil_value_mask                              = GL_STENCIL_VALUE_MASK,
        stencil_writemask                               = GL_STENCIL_WRITEMASK,
        subpixel_bits                                   = GL_SUBPIXEL_BITS,
        texture_binding_1d                              = GL_TEXTURE_BINDING_1D,
        texture_binding_1d_array                        = GL_TEXTURE_BINDING_1D_ARRAY,
        texture_binding_2d                              = GL_TEXTURE_BINDING_2D,
        texture_binding_2d_array                        = GL_TEXTURE_BINDING_2D_ARRAY,
        texture_binding_2d_multisample                  = GL_TEXTURE_BINDING_2D_MULTISAMPLE,
        texture_binding_2d_multisample_array            = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
        texture_binding_3d                              = GL_TEXTURE_BINDING_3D,
        texture_binding_buffer                          = GL_TEXTURE_BINDING_BUFFER,
        texture_binding_cube_map                        = GL_TEXTURE_BINDING_CUBE_MAP,
        texture_binding_cube_map_array                  = GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,
        texture_binding_rectangle                       = GL_TEXTURE_BINDING_RECTANGLE,
        texture_buffer_offset_alignment                 = GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT,
        texture_compression_hint                        = GL_TEXTURE_COMPRESSION_HINT,
        transform_feedback_buffer_binding               = GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
        uniform_buffer_binding                          = GL_UNIFORM_BUFFER_BINDING,
        uniform_buffer_offset_alignment                 = GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
        unpack_alignment                                = GL_UNPACK_ALIGNMENT,
        unpack_image_height                             = GL_UNPACK_IMAGE_HEIGHT,
        unpack_row_length                               = GL_UNPACK_ROW_LENGTH,
        unpack_skip_images                              = GL_UNPACK_SKIP_IMAGES,
        unpack_skip_pixels                              = GL_UNPACK_SKIP_PIXELS,
        unpack_skip_rows                                = GL_UNPACK_SKIP_ROWS,
        vertex_array_binding                            = GL_VERTEX_ARRAY_BINDING,
        vertex_binding_buffer                           = GL_VERTEX_BINDING_BUFFER,
        vertex_binding_divisor                          = GL_VERTEX_BINDING_DIVISOR,
        vertex_binding_offset                           = GL_VERTEX_BINDING_OFFSET,
        vertex_binding_stride                           = GL_VERTEX_BINDING_STRIDE,
    };

    enum class int64_names {
        max_server_wait_timeout = GL_MAX_SERVER_WAIT_TIMEOUT,
        timestamp               = GL_TIMESTAMP,
    };

    enum class bool_names {
        blend                      = GL_BLEND,
        color_logic_op             = GL_COLOR_LOGIC_OP,
        cull_face                  = GL_CULL_FACE,
        depth_test                 = GL_DEPTH_TEST,
        depth_writemask            = GL_DEPTH_WRITEMASK,
        dither                     = GL_DITHER,
        doublebuffer               = GL_DOUBLEBUFFER,
        line_smooth                = GL_LINE_SMOOTH,
        pack_lsb_first             = GL_PACK_LSB_FIRST,
        pack_swap_bytes            = GL_PACK_SWAP_BYTES,
        polygon_offset_fill        = GL_POLYGON_OFFSET_FILL,
        polygon_offset_line        = GL_POLYGON_OFFSET_LINE,
        polygon_offset_point       = GL_POLYGON_OFFSET_POINT,
        polygon_smooth             = GL_POLYGON_SMOOTH,
        program_point_size         = GL_PROGRAM_POINT_SIZE,
        sample_coverage_invert     = GL_SAMPLE_COVERAGE_INVERT,
        scissor_test               = GL_SCISSOR_TEST,
        shader_compiler            = GL_SHADER_COMPILER,
        stencil_test               = GL_STENCIL_TEST,
        stereo                     = GL_STEREO,
        unpack_lsb_first           = GL_UNPACK_LSB_FIRST,
        unpack_swap_bytes          = GL_UNPACK_SWAP_BYTES,
    };

    enum class floating_point_names {
        depth_clear_value                 = GL_DEPTH_CLEAR_VALUE,
        line_width                        = GL_LINE_WIDTH,
        line_width_granularity            = GL_LINE_WIDTH_GRANULARITY,
        max_texture_lod_bias              = GL_MAX_TEXTURE_LOD_BIAS,
        min_fragment_interpolation_offset = GL_MIN_FRAGMENT_INTERPOLATION_OFFSET,
        polygon_offset_clamp              = GL_POLYGON_OFFSET_CLAMP,
        polygon_offset_factor             = GL_POLYGON_OFFSET_FACTOR,
        polygon_offset_units              = GL_POLYGON_OFFSET_UNITS,
        point_fade_threshold_size         = GL_POINT_FADE_THRESHOLD_SIZE,
        point_size                        = GL_POINT_SIZE,
        point_size_granularity            = GL_POINT_SIZE_GRANULARITY,
        sample_coverage_value             = GL_SAMPLE_COVERAGE_VALUE,
        smooth_line_width_granularity     = GL_SMOOTH_LINE_WIDTH_GRANULARITY,
    };

    enum class paired_int_names {
        max_viewport_dims            = GL_MAX_VIEWPORT_DIMS,
        viewport_bounds_range        = GL_VIEWPORT_BOUNDS_RANGE,
    };

    enum class paired_float_names {
        aliased_line_width_range     = GL_ALIASED_LINE_WIDTH_RANGE,
        depth_range                  = GL_DEPTH_RANGE,
        point_size_range             = GL_POINT_SIZE_RANGE,
        smooth_line_width_range      = GL_SMOOTH_LINE_WIDTH_RANGE,
    };

    enum class quadruple_floating_point_names {
        blend_color                  = GL_BLEND_COLOR,
        color_clear_value            = GL_COLOR_CLEAR_VALUE,
    };

    enum class quadruple_int_names {
        scissor_box                  = GL_SCISSOR_BOX,
        viewport                     = GL_VIEWPORT,
    };

    enum class quadruple_bool_names {
        color_writemask              = GL_COLOR_WRITEMASK,
    };

    enum class multiple_int_names {
        compressed_texture_formats   = GL_COMPRESSED_TEXTURE_FORMATS,

    };

    template<std::derived_from<context_base> Context>
    [[nodiscard]]
    GLint get(const Context& ctx, int_names name) {
        GLint val{};
        gl_function{&GladGLContext::GetIntegerv}(ctx, to_underlying_value(name), &val);
        return val;
    }

    template<std::derived_from<context_base> Context>
    [[nodiscard]]
    GLboolean get(const Context& ctx, bool_names name) {
        GLboolean val{};
        gl_function{&GladGLContext::GetBooleanv}(ctx, to_underlying_value(name), &val);
        return val;
    }

    template<gl_floating_point T, std::derived_from<context_base> Context>
    [[nodiscard]]
    T get(const Context& ctx, floating_point_names name) {
        T val{};
        if constexpr(std::same_as<T, GLfloat>)
            gl_function{&GladGLContext::GetFloatv}(ctx, to_underlying_value(name), &val);
        else
            gl_function{&GladGLContext::GetDoublev}(ctx, to_underlying_value(name), &val);

        return val;
    }

    template<gl_floating_point T, std::derived_from<context_base> Context>
    [[nodiscard]]
    std::array<T, 4> get(const Context& ctx, quadruple_floating_point_names name) {
        std::array<T, 4> vals{};
        if constexpr(std::same_as<T, GLfloat>)
            gl_function{&GladGLContext::GetFloatv}(ctx, to_underlying_value(name), vals.data());
        else
            gl_function{&GladGLContext::GetDoublev}(ctx, to_underlying_value(name), vals.data);

        return vals;
    }
}

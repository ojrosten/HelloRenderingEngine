////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Core/ShaderCompilation/ShaderCompiler.hpp"

#include"sequoia/Streaming/Streaming.hpp"

#include <format>

namespace avocet {
    namespace {
        [[nodiscard]]
        std::vector<uint32_t> do_compile_glsl_to_spv(std::string_view source, shaderc_shader_kind kind, std::string_view label) {
            shaderc::CompileOptions options;

            options.SetOptimizationLevel(shaderc_optimization_level_performance);

            shaderc::SpvCompilationResult module{
                shaderc::Compiler{}.CompileGlslToSpv(source.data(), kind, label.data(), options)
            };

            if(module.GetCompilationStatus() != shaderc_compilation_status_success) {
                throw std::runtime_error{std::format("Unable to compile to spv: {}", module.GetErrorMessage())};
            }

            return {std::from_range, module};
        }
    }

    [[nodiscard]]
    std::vector<uint32_t> compile_glsl_to_spv(const std::filesystem::path& shaderPath, shaderc_shader_kind kind, std::string_view label) {
        const auto optContent{sequoia::read_to_string(shaderPath)};
        if(!optContent)
            throw std::runtime_error{std::format("Unable to read file {}", shaderPath.generic_string())};

        return do_compile_glsl_to_spv(optContent.value(), kind, label);
    }
}
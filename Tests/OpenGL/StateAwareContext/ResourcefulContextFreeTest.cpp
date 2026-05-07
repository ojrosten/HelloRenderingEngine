////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ResourcefulContextFreeTest.hpp"
#include "avocet/OpenGL/ResourceInfrastructure/Labels.hpp"
#include "avocet/OpenGL/StateAwareContext/ResourcefulContext.hpp"

#include "curlew/Window/WindowConfigurations.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    namespace {
        [[nodiscard]]
        curlew::window_config make_window_config(std::vector<std::string>& calls) {
            return curlew::make_call_logging_window_config({.width{1}, .height{1}}, calls, [](const agl::decorator_data& data) { return data.fn_name == "GetIntegerv"; });
        }
    }

    [[nodiscard]]
    std::filesystem::path resourceful_context_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void resourceful_context_free_test::run_tests()
    {
        auto checkMaxLabelLenCall{
            [this](std::source_location loc=std::source_location::current()) {
                std::vector<std::string> calls{};
                auto win{create_window(make_window_config(calls))};

                if(const auto optLen{agl::get_max_label_length(win.context())}; optLen)
                    check(equivalence, reporter{"", loc}, calls, std::array{std::string_view{"GetIntegerv"}});
            }
        };

        checkMaxLabelLenCall();
        //checkMaxLabelLenCall();
    }
}

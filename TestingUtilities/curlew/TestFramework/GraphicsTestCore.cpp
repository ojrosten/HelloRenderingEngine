////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////


#include "curlew/TestFramework/GraphicsTestCore.hpp"

namespace curlew {
    namespace {
        std::string& erase_backwards(std::string& message, std::string_view pattern) {
            constexpr auto npos{std::string::npos};
            if(const auto rightPos{message.find(pattern)}; rightPos < npos) {
                if(const auto leftPos{message.rfind(" ", rightPos)}; leftPos < rightPos)
                    message.erase(leftPos + 1, rightPos - leftPos - 1);
            }

            return message;
        }
    }

    [[nodiscard]]
    std::string exception_postprocessor::operator()(std::string message) const {
        erase_backwards(message, "Tests");
        erase_backwards(message, "Source/avocet");

        return message;
    }
}
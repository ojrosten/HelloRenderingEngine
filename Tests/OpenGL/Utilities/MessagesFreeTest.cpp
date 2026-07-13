////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "MessagesFreeTest.hpp"
#include "avocet/OpenGL/Utilities/Messages.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path messages_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void messages_free_test::run_tests()
    {
        using opengl::trim;

        check_exception_thrown<std::domain_error>(
            "",
            []() {
                std::string message{};
                trim(message, -1);
            }
        );

        check_exception_thrown<std::logic_error>(
            "",
            []() {
                std::string message{};
                trim(message, 1);
            }
        );

        auto check_trim{
            [this](std::string paddedMessage, GLsizei actualMessageLength, const std::string& prediction) {
                check(equality, "", trim(paddedMessage, actualMessageLength), prediction);
            }
        };

        check_trim(std::string{}, 0, std::string{});
        check_trim(std::string{"Hello, World"},  0, std::string{});
        check_trim(std::string{"Hello, World"},  5, std::string{"Hello"});
        check_trim(std::string{"Hello, World"}, 12, std::string{"Hello, World"});

        check_trim([]() {std::string s{}; s.push_back('\0'); return s; }(), 0, std::string{});
        check_trim([]() {std::string s{}; s.push_back('\0'); return s; }(), 1, std::string{});

        check_trim([]() {std::string s{"Hello"}; s.push_back('\0'); return s; }(), 6, std::string{"Hello"});
    }
}

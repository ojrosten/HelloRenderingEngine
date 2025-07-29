////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramThreadingFreeTest.hpp"
#include "avocet/OpenGL/Graphics/ShaderProgram.hpp"

#include <future>
#include <latch>
#include <thread>

namespace avocet::testing
{
    namespace agl = avocet::opengl;
    namespace fs = std::filesystem;

    namespace
    {
        agl::resource_handle get_program_index() {
            GLint param{};
            agl::gl_function{glGetIntegerv}(GL_CURRENT_PROGRAM, &param);
            if(param < 0)
                throw std::runtime_error{"Negative program index!"};

            return agl::resource_handle{static_cast<GLuint>(param)};
        }

        agl::resource_handle make_and_use_shader_program(curlew::glfw_manager& manager, const fs::path& shaderDir, std::latch* pLatch) {
            auto w{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
            agl::shader_program sp{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs", w.context_index()};
            sp.use(w.context_index());
            if(pLatch) pLatch->arrive_and_wait();
            return get_program_index();
        }

        constexpr std::latch* no_latch{};

        [[nodiscard]]
        std::string make_description(std::string_view tag, std::string_view description)
        {
            return std::format("{}: {}", tag, description);
        }
    }

    [[nodiscard]]
    std::filesystem::path shader_program_threading_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_threading_free_test::run_tests()
    {
        using namespace curlew;
        curlew::glfw_manager manager{};

        check_serial_tracking(manager);
        check_serial_tracking_overlapping_lifetimes(manager);
        check_serial_shared_context_tracking(manager);
        check_threaded_tracking(manager);
        check_threaded_shared_context_tracking(manager);
    }

    void shader_program_threading_free_test::check_serial_tracking(curlew::glfw_manager& manager)
    {
        const auto shaderDir{working_materials()};
        const auto
            prog0{make_and_use_shader_program(manager, shaderDir, no_latch)},
            prog1{make_and_use_shader_program(manager, shaderDir, no_latch)};

        check_program_indices(prog0, prog1, "serial");
    }

    void shader_program_threading_free_test::check_serial_tracking_overlapping_lifetimes(curlew::glfw_manager& manager)
    {
        const auto shaderDir{working_materials()};
        auto w{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
        agl::shader_program sp{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs", w.context_index()};
        sp.use(w.context_index());
        const auto prog0{get_program_index()};

        auto w1{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
        agl::shader_program sp1{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs", w1.context_index()};
        sp1.use(w1.context_index());
        const auto prog1{get_program_index()};

        check_program_indices(prog0, prog1, "serial overlapping");
    }

    void shader_program_threading_free_test::check_serial_shared_context_tracking(curlew::glfw_manager& manager)
    {
        const auto shaderDir{working_materials()};
        auto w{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
        agl::shader_program sp{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs", w.context_index()};
        sp.use(w.context_index());
        const auto prog0{get_program_index()};

        auto w1{manager.create_window({.hiding{curlew::window_hiding_mode::on}}, w)};
        sp.use(w1.context_index());
        const auto sharedProg0{get_program_index()};

        check("", prog0 == sharedProg0);
    }


    void shader_program_threading_free_test::check_threaded_tracking(curlew::glfw_manager& manager)
    {
        const auto shaderDir{working_materials()};
        std::latch holdYourHorses{2};
        std::packaged_task<agl::resource_handle(void)>
            task0{[&]() { return make_and_use_shader_program(manager, shaderDir, &holdYourHorses); } },
            task1{[&]() { return make_and_use_shader_program(manager, shaderDir, &holdYourHorses); } };

        auto future0{task0.get_future()};
        std::jthread worker0{std::move(task0)};

        auto future1{task1.get_future()};
        std::jthread worker1{std::move(task1)};

        const auto prog0{future0.get()};
        const auto prog1{future1.get()};

        check_program_indices(prog0, prog1, "threaded");
    }

    void shader_program_threading_free_test::check_program_indices(const avocet::opengl::resource_handle& prog0, const avocet::opengl::resource_handle& prog1, std::string_view tag)
    {
        if(check(make_description(tag, "Bounded Context: Either one of the programs isn't utilized or they both are and have the same index"), (!prog0 || !prog1 || (prog0 == prog1))))
        {
            check(make_description(tag, "prog0 should report > 0"), prog0.index() > 0);
            check(make_description(tag, "prog1 should report > 0"), prog1.index() > 0);
        }
    }

    void shader_program_threading_free_test::check_threaded_shared_context_tracking(curlew::glfw_manager& manager)
    {
    }
}

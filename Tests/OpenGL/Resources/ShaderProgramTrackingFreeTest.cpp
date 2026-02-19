////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramTrackingFreeTest.hpp"

#include "OpenGL/ResourceInfrastructure/ResourceHandleTestingUtilities.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

#include <future>
#include <thread>
#include <latch>

namespace avocet::testing
{
    namespace agl = avocet::opengl;
    namespace fs = std::filesystem;

    namespace
    {
        class call_logger {
            std::vector<std::string>* m_Calls{};
        public:
            call_logger(std::vector<std::string>& calls)
                : m_Calls{&calls}
            {}

            void operator()(const agl::context&, const agl::decorator_data& data) {
                if(data.fn_name == "UseProgram")
                    m_Calls->push_back(std::string{data.fn_name});
            }
        };

        [[nodiscard]]
        curlew::window_config make_window_config(std::vector<std::string>& loggedCalls) {
            return {
                .extent{.width{1}, .height{1}},
                .name{},
                .hiding{curlew::window_hiding_mode::on},
                .debug_mode{agl::debugging_mode::dynamic},
                .prologue{call_logger{loggedCalls}},
                .epilogue{agl::standard_error_checker{agl::num_messages{10}, agl::default_debug_info_processor{}}},
                .compensate{agl::attempt_to_compensate_for_driver_bugs::yes},
                .samples{1}
            };
        }

        [[nodiscard]]
        agl::resource_handle get_current_program_index(const agl::decorated_context& ctx) {
            GLint param{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, GL_CURRENT_PROGRAM, &param);
            if(param < 0)
                throw std::runtime_error{std::format("Negative program index: {}", param)};

            return agl::resource_handle{static_cast<GLuint>(param)};
        }

        constexpr std::latch* no_latch{};

        [[nodiscard]]
        agl::resource_handle make_and_use_shader_program(const curlew::window& w,
                                                         const fs::path& shaderDir,
                                                         std::latch* entryLatch,
                                                         std::latch* exitLatch)
        {
            const auto& ctx{w.context()};

            agl::shader_program sp{
                ctx,
                shaderDir / "Identity.vs",
                shaderDir / "Monochrome.fs"
            };

            if(entryLatch) entryLatch->arrive_and_wait();

            sp.use();
            auto progIndex{get_current_program_index(ctx)};
            sp.use();

            if(exitLatch) exitLatch->arrive_and_wait();

            return progIndex;
        }

        [[nodiscard]]
        agl::resource_handle make_and_use_shader_program(const curlew::window& w, const fs::path& shaderDir) {
            return make_and_use_shader_program(w, shaderDir, no_latch, no_latch);
        }

        [[nodiscard]]
        std::packaged_task<agl::resource_handle()> make_shader_program_task(curlew::window& w,
                                                                            const fs::path& shaderDir,
                                                                            std::latch* entryLatch,
                                                                            std::latch* exitLatch)
        {
            curlew::test_window_manager::detach_current_context();

            return
                std::packaged_task<agl::resource_handle()>{
                    [&, entryLatch, exitLatch]() {
                        w.make_context_current();
                        return make_and_use_shader_program(w, shaderDir, entryLatch, exitLatch);
                    }
                };
        }

        [[nodiscard]]
        agl::resource_handle make_and_use_shader_program_threaded(curlew::window& w,
                                                                  const fs::path& shaderDir)
        {
            auto task{make_shader_program_task(w, shaderDir, no_latch, no_latch)};
            auto fut{task.get_future()};
            std::jthread executor{std::move(task)};

            return fut.get();
        }

        [[nodiscard]]
        std::string make_description(std::string_view tag, std::string_view description)
        {
            return std::format("{}\n{}", tag, description);
        }
    }

    [[nodiscard]]
    std::filesystem::path shader_program_tracking_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_tracking_free_test::run_tests()
    {
        check_serial_tracking_non_overlapping_lifetimes();
        check_serial_tracking_overlapping_lifetimes();

        check_parallel_tracking_non_overlapping_lifetimes();
        check_parallel_tracking_overlapping_lifetimes();
    }

    void shader_program_tracking_free_test::check_serial_tracking_non_overlapping_lifetimes()
    {
        std::vector<std::string> calls0{}, calls1{};

        const auto shaderDir{working_materials()};
        const auto prog0{make_and_use_shader_program(create_window(make_window_config(calls0)), shaderDir)},
                   prog1{make_and_use_shader_program(create_window(make_window_config(calls1)), shaderDir)};

        check_program_indices(report("Serial non-overlapping lifetimes"), prog0, prog1, calls0, calls1);
    }

    void shader_program_tracking_free_test::check_serial_tracking_overlapping_lifetimes()
    {
        std::vector<std::string> calls0{}, calls1{};

        const auto shaderDir{working_materials()};
        const auto win0{create_window(make_window_config(calls0))};
        agl::shader_program sp1{win0.context(), shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
        sp1.use();
        const auto prog0{get_current_program_index(win0.context())};

        const auto win1{create_window(make_window_config(calls1))};
        agl::shader_program sp2{win1.context(), shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
        sp2.use();
        const auto prog1{get_current_program_index(win1.context())};

        check_program_indices(report("Serial overlapping lifetimes"), prog0, prog1, calls0, calls1);
    }

    void shader_program_tracking_free_test::check_parallel_tracking_non_overlapping_lifetimes()
    {
        std::vector<std::string> calls0{}, calls1{};

        const auto shaderDir{working_materials()};
        auto win0{create_window(make_window_config(calls0))},
             win1{create_window(make_window_config(calls1))};

        const auto prog0{make_and_use_shader_program_threaded(win0, shaderDir)},
                   prog1{make_and_use_shader_program_threaded(win1, shaderDir)};

        check_program_indices(report("Parallel non-overlapping lifetimes"), prog0, prog1, calls0, calls1);
    }

     void shader_program_tracking_free_test::check_parallel_tracking_overlapping_lifetimes()
     {
         std::vector<std::string> calls0{}, calls1{};

         const auto shaderDir{working_materials()};
         auto win0{create_window(make_window_config(calls0))},
              win1{create_window(make_window_config(calls1))};

         std::latch entryLatch{2}, exitLatch{2};

         auto task0{make_shader_program_task(win0, shaderDir, &entryLatch, &exitLatch)},
              task1{make_shader_program_task(win1, shaderDir, &entryLatch, &exitLatch)};

         auto fut0{task0.get_future()},
              fut1{task1.get_future()};

         std::array workers{std::jthread{std::move(task0)}, std::jthread{std::move(task1)}};

         check_program_indices(report("Parallel overlapping lifetimes"), fut0.get(), fut1.get(), calls0, calls1);
     }

    void shader_program_tracking_free_test::check_program_indices(std::string_view tag, const avocet::opengl::resource_handle& prog0, const avocet::opengl::resource_handle& prog1, const std::vector<std::string>& calls0, const std::vector<std::string>& calls1)
    {
        check(make_description(tag, "prog0 should not be null"), prog0 != agl::resource_handle{});
        check(make_description(tag, "prog1 should not be null"), prog1 != agl::resource_handle{});

        check(equality, make_description(tag, "Assumption required for sensitivity to: program 0 utilization accidentally suppressing program 1 utilization"), prog0, prog1);
        check(equivalence, make_description(tag, "Single dispatch to UseProgram"), calls0, std::initializer_list{"UseProgram"});
        check(equivalence, make_description(tag, "Single dispatch to UseProgram"), calls1, std::initializer_list{"UseProgram"});
    }
}

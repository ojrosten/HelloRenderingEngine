////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Utilities/Casts.hpp"
#include "avocet/OpenGL/Graphics/GLFunction.hpp"
#include "avocet/OpenGL/Graphics/ObjectIdentifiers.hpp"
#include "avocet/OpenGL/Graphics/ResourceHandle.hpp"

#include <optional>

namespace avocet::opengl {
    using optional_label = std::optional<std::string>;
    inline constexpr optional_label null_label{std::nullopt};

    inline void add_label(object_identifier identifier, const resource_handle& h, const optional_label& label) {
        if(label && object_labels_activated()) {
            const auto& str{label.value()};
            gl_function{glObjectLabel}(to_gl_enum(identifier), h.index(), to_gl_sizei(str.size()), str.data());
        }
    }

    [[nodiscard]]
    inline GLint get_max_label_length() {
        const static GLint length{
            [](){
                GLint param{};
                gl_function{glGetIntegerv}(GL_MAX_LABEL_LENGTH, &param);
                return param;
            }()
        };

        return length;
    }

    [[nodiscard]]
    inline std::string get_object_label(avocet::opengl::object_identifier identifier, const avocet::opengl::resource_handle& handle) {
        std::string label(get_max_label_length(), ' ');
        GLsizei numChars{};
        gl_function{glGetObjectLabel}(
            to_gl_enum(identifier),
            handle.index(),
            to_gl_sizei(label.size()),
            &numChars,
            label.data()
            );

        label.erase(std::ranges::next(label.begin(), numChars, label.end()), label.end());

        return label;
    }
}
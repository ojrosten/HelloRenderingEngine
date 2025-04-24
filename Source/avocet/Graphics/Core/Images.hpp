////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <filesystem>
#include <span>

namespace avocet {

    enum class vertical_flip : bool { no, yes };

    enum class colour_space_flavour : bool { linear, gamma };

    struct image_configuration
    {
        std::filesystem::path file{};
        vertical_flip flip{};
        colour_space_flavour colour_space{colour_space_flavour::gamma};

        [[nodiscard]]
        friend bool operator==(const image_configuration&, const image_configuration&) = default;
    };

    struct image
    {
        std::span<unsigned char> data;
        int width{}, height{}, num_channels{};
    };

    class [[nodiscard]] image_loader{
        image m_Image;

        [[nodiscard]]
        static image make(const std::filesystem::path& texture, vertical_flip flip);
    public:
        image_loader(const std::filesystem::path& texture, vertical_flip flip);

        ~image_loader();

        [[nodiscard]]
        const image& get_image() const noexcept { return m_Image; }
    };
}
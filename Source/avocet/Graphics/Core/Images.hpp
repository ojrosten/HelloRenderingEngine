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

    enum class vertically_flipped : bool { no, yes };

    enum class colour_space_flavour : bool { linear, gamma };

    class image_configuration
    {
        std::filesystem::path m_File{};
        vertically_flipped m_Flip{};
        colour_space_flavour m_ColourSpace{colour_space_flavour::gamma};
    public:
        image_configuration(std::filesystem::path textureFile, vertically_flipped flip, colour_space_flavour colourSpace)
            : m_File{std::move(textureFile)}
            , m_Flip{flip}
            , m_ColourSpace{colourSpace}
        {}

        [[nodiscard]]
        const std::filesystem::path& file() const noexcept { return m_File; }

        [[nodiscard]]
        vertically_flipped flipped() const noexcept { return  m_Flip; }

        [[nodiscard]]
        colour_space_flavour colour_space() const noexcept { return m_ColourSpace; }


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
        static image make(const std::filesystem::path& texture, vertically_flipped flip);
    public:
        image_loader(const std::filesystem::path& texture, vertically_flipped flip);

        ~image_loader();

        [[nodiscard]]
        const image& get_image() const noexcept { return m_Image; }
    };
}
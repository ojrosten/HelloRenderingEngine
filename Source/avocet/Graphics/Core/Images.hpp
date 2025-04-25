////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <filesystem>
#include <format>
#include <span>
#include <source_location>

namespace avocet {

    enum class vertically_flipped : bool { no, yes };

    enum class colour_space_flavour : bool { linear, gamma };

    class image_configuration
    {
        std::filesystem::path m_File{};
        vertically_flipped m_Flip{};
        colour_space_flavour m_ColourSpace{colour_space_flavour::gamma};
    public:
        image_configuration(std::filesystem::path textureFile, vertically_flipped flip, colour_space_flavour colourSpace, std::source_location loc=std::source_location::current())
            : m_File{std::move(textureFile)}
            , m_Flip{flip}
            , m_ColourSpace{colourSpace}
        {
            if(!std::filesystem::exists(m_File))
                throw std::runtime_error{std::format("image_configuration: file {} requested from {}, line {} does not exist", m_File.generic_string(), loc.file_name(), loc.line())};
        }

        [[nodiscard]]
        const std::filesystem::path& file() const noexcept { return m_File; }

        [[nodiscard]]
        vertically_flipped flipped() const noexcept { return  m_Flip; }

        [[nodiscard]]
        colour_space_flavour colour_space() const noexcept { return m_ColourSpace; }


        [[nodiscard]]
        friend bool operator==(const image_configuration&, const image_configuration&) = default;
    };

    class image_view
    {
    public:
        using data_type = unsigned char;

        image_view(data_type* ptr, int width, int height, int numChannels)
            : m_Data{ptr}
            , m_Width{to_unsigned(width, "width")}
            , m_Height{to_unsigned(height, "height")}
            , m_NumChannels{to_unsigned(numChannels, "channels")}
        {}

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Width; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Height; }

        std::size_t num_channels() const noexcept { return m_NumChannels; }

        [[nodiscard]]
        std::span<const data_type> span() const noexcept { return {m_Data, size()}; }

        [[nodiscard]]
        std::span<data_type> span() noexcept { return {m_Data, size()}; }
    private:
        data_type* m_Data;
        std::size_t m_Width{}, m_Height{}, m_NumChannels{};

        [[nodiscard]]
        static std::size_t to_unsigned(int val, std::string_view name) {
            if(val < 0) throw std::runtime_error{std::format("image_view: {} = {}, but it should be positive", name, val)};

            return static_cast<std::size_t>(val);
        }

        [[nodiscard]]
        std::size_t size() const noexcept { return width() * height() * num_channels() * sizeof(data_type);  }
    };

    class [[nodiscard]] image_loader{
        image_view m_Image;

        [[nodiscard]]
        static image_view make(const std::filesystem::path& texture, vertically_flipped flip);
    public:
        image_loader(const std::filesystem::path& texture, vertically_flipped flip);

        ~image_loader();

        [[nodiscard]]
        image_view get_image() const noexcept { return m_Image; }
    };
}
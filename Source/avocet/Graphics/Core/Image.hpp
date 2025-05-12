////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <filesystem>
#include <format>
#include <memory>
#include <mutex>
#include <span>
#include <source_location>

namespace avocet {

    enum class flip_vertically : bool { no, yes };

    enum class colour_space_flavour : bool { linear, gamma };

    class image_configuration
    {
        std::filesystem::path m_File{};
        flip_vertically m_Flip{};
        colour_space_flavour m_ColourSpace{colour_space_flavour::gamma};
    public:
        image_configuration(std::filesystem::path textureFile, flip_vertically flip, colour_space_flavour colourSpace, std::source_location loc=std::source_location::current())
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
        flip_vertically flipped() const noexcept { return  m_Flip; }

        [[nodiscard]]
        colour_space_flavour colour_space() const noexcept { return m_ColourSpace; }


        [[nodiscard]]
        friend bool operator==(const image_configuration&, const image_configuration&) = default;
    };

    class image {
        using data_type = unsigned char;

        struct file_deleter{
            void operator()(data_type* ptr) const;
        };

        struct parameter {
            std::size_t value{};

            explicit parameter(std::size_t val) : value{val} {}

            parameter(parameter&& other) : value{std::exchange(other.value, 0)} {}

            parameter& operator=(parameter&& other) {
                value = std::exchange(other.value, 0);
                return *this;
            }

            [[nodiscard]]
            friend auto operator<=>(const parameter&, const parameter&) noexcept = default;
        };

        [[nodiscard]]
        static image make(const std::filesystem::path& texture, flip_vertically flip);

        parameter m_Width, m_Height, m_NumChannels;
        std::unique_ptr<data_type, file_deleter> m_Data;
        inline static std::mutex st_Mutex{};

        [[nodiscard]]
        std::size_t size() const noexcept { return width() * height() * num_channels(); }

        image(std::size_t width, std::size_t height, std::size_t numChannels, data_type* ptr)
            : m_Width{width}
            , m_Height{height}
            , m_NumChannels{numChannels}
            , m_Data{ptr}
        {}

    public:
        image(const std::filesystem::path& texture, flip_vertically flip)
            : image{make(texture, flip)}
        {}

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Width.value; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Height.value; }

        std::size_t num_channels() const noexcept { return m_NumChannels.value; }

        [[nodiscard]]
        std::span<const data_type> span() const noexcept { return {m_Data.get(), size()}; }

        [[nodiscard]]
        friend bool operator==(const image&, const image&) noexcept = default;
    };
}

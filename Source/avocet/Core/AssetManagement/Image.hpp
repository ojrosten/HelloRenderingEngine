////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <filesystem>
#include <format>
#include <memory>
#include <mutex>
#include <span>
#include <utility>

namespace avocet {
    enum class flip_vertically : bool { no, yes };

    class image_channels {
        std::size_t m_Value{};
    public:
        constexpr image_channels() noexcept {};

        constexpr explicit image_channels(std::size_t n)
            : m_Value{n}
        {}

        [[nodiscard]]
        friend constexpr auto operator<=>(const image_channels&, const image_channels&) noexcept = default;

        [[nodiscard]]
        std::size_t raw_value() const noexcept { return m_Value; }

        [[nodiscard]]
        explicit operator std::size_t() const noexcept { return m_Value; }
    };

    class image {
    public:
        using value_type = unsigned char;

        image(const std::filesystem::path& texturePath, flip_vertically flip)
            : image{make(texturePath, flip)}
        {}

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Width.value; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Height.value; }

        [[nodiscard]]
        image_channels num_channels() const noexcept { return m_Channels.value; }

        [[nodiscard]]
        std::size_t size() const noexcept { return width() * height() * num_channels().raw_value(); }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return {m_Data.get(), size()}; }

        [[nodiscard]]
        friend bool operator==(const image&, const image&) noexcept = default;
    private:
        struct file_unloader {
            void operator()(value_type* ptr) const;
        };

        template<class T>
        struct parameter {
            [[nodiscard]]
            std::size_t to_unsigned(int val) {
                if(val < 0)
                    throw std::logic_error{std::format("image::parameter - negative value {}", val)};

                return static_cast<std::size_t>(val);
            }

            T value{};

            explicit parameter(int val)
                : value{to_unsigned(val)}
            { }

            parameter(parameter&& other) noexcept 
                : value{std::exchange(other.value, T{})}
            {}

            parameter& operator=(parameter&& other) noexcept {
                if(this != &other)
                    value = std::exchange(other.value, T{});

                return *this;
            }

            [[nodiscard]]
            auto operator<=>(const parameter&) const noexcept = default;
        };

        std::unique_ptr<value_type, file_unloader> m_Data;
        parameter<std::size_t> m_Width, m_Height;
        parameter<image_channels> m_Channels;

        image(value_type* ptr, int width, int height, int channels)
            : m_Data{ptr}
            , m_Width{width}
            , m_Height{height}
            , m_Channels{channels}
        {}

        [[nodiscard]]
        static image make(const std::filesystem::path& texturePath, flip_vertically flip);
    };

    class image_view {
    public:
        using value_type = unsigned char;

        image_view(std::span<const value_type> data, std::size_t width, std::size_t height, std::size_t numChannels)
            : m_Data{data}
            , m_Width{width}
            , m_Height{height}
        {
            if(m_Width * m_Height * numChannels != m_Data.size())
                throw std::runtime_error{std::format("image_view: image size {} not a multiple of the width, height and channels, {} * {} * {}", m_Data.size(), m_Width, m_Height, numChannels)};
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Width; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Height; }

        [[nodiscard]]
        std::size_t num_channels() const noexcept { return m_Data.size() / (width() * height()); }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return m_Data; }

        [[nodiscard]]
        friend bool operator==(const image&, const image&) noexcept = default;
    private:
        std::span<const value_type> m_Data;
        std::size_t m_Width{}, m_Height{};
    };
}

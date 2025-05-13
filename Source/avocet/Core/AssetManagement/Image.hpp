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
#include <optional>
#include <span>
#include <variant>
#include <utility>

#include "sequoia/Core/Meta/Utilities.hpp"

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

    class alignment {
        std::size_t m_Value{1};

        [[nodiscard]]
        constexpr static bool is_power_of_2(std::size_t val) {
            return (val > 0) && ((val & (val - 1)) == 0);
        }
    public:
        constexpr alignment() noexcept {};

        constexpr explicit alignment(std::size_t n)
            : m_Value{n}
        {
            if(!is_power_of_2(m_Value))
                throw std::runtime_error{std::format("alignment: value {} is not a power of 2", m_Value)};
        }

        [[nodiscard]]
        friend constexpr auto operator<=>(const alignment&, const alignment&) noexcept = default;

        [[nodiscard]]
        std::size_t raw_value() const noexcept { return m_Value; }

        [[nodiscard]]
        explicit operator std::size_t() const noexcept { return m_Value; }
    };

    inline constexpr std::optional<image_channels> channels_in_image{std::nullopt};


    class image {
    public:
        using value_type = unsigned char;

        image(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<image_channels> requestedChannels)
            : image{make(texturePath, flip, requestedChannels)}
        {}

        image(std::vector<value_type> data, std::size_t imageWidth, std::size_t imageHeight, image_channels channels, alignment rowAlignment)
            : m_Data{std::move(data)}
            , m_Width{imageWidth}
            , m_Height{imageHeight}
            , m_Channels{channels}
            , m_Alignment{rowAlignment}
        {
            if(const auto sz{std::get<vec_t>(m_Data).size()}; height() * aligned_row_size() != sz)
                throw std::runtime_error{std::format("image: image size {} not a multiple of the height and padded row size, {}, {}", sz, height(), aligned_row_size())};
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Width.value; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Height.value; }

        [[nodiscard]]
        image_channels num_channels() const noexcept { return m_Channels.value; }

        [[nodiscard]]
        std::size_t size() const noexcept { return height() * aligned_row_size(); }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Alignment.value; }

        [[nodiscard]]
        std::size_t aligned_row_size() const noexcept {
            const std::size_t excess{(width() * num_channels().raw_value()) % row_alignment().raw_value()};
            const auto alignedWidth{excess ? width() - excess + row_alignment().raw_value() : width()};
            return alignedWidth * num_channels().raw_value();
        }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept {
            using span_t = std::span<const value_type>;
            return std::visit(
                sequoia::overloaded{
                    [this](const ptr_t& p) { return span_t{p.get(), size()}; },
                    []    (const vec_t& v) { return span_t{v}; }
                },
                m_Data
            );
        }

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

            explicit parameter(T val) noexcept
                : value{val}
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

        using ptr_t = std::unique_ptr<value_type, file_unloader>;
        using vec_t = std::vector<value_type>;
        std::variant<ptr_t, vec_t> m_Data;
        parameter<std::size_t> m_Width, m_Height;
        parameter<image_channels> m_Channels;
        parameter<alignment> m_Alignment;

        image(value_type* ptr, int width, int height, int channels, alignment rowAlignment)
            : m_Data{ptr_t{ptr}}
            , m_Width{width}
            , m_Height{height}
            , m_Channels{channels}
            , m_Alignment{rowAlignment}
        {}

        [[nodiscard]]
        static image make(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<image_channels> requestedChannels);
    };

    class image_view {
    public:
        using value_type = unsigned char;

        image_view(std::span<const value_type> data, std::size_t imageWidth, std::size_t imageHeight, image_channels numChannels, alignment rowAlignment)
            : m_Data{data}
            , m_Width{imageWidth}
            , m_Height{imageHeight}
            , m_Channels{numChannels}
            , m_Alignment{rowAlignment}
        {
            if(height() * aligned_row_size() != m_Data.size())
                throw std::runtime_error{std::format("image: image size {} not a multiple of the height and padded row size, {}, {}", m_Data.size(), height(), aligned_row_size())};
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Width; }

        [[nodiscard]]
        std::size_t aligned_row_size() const noexcept {
            const std::size_t excess{(width() * num_channels().raw_value()) % row_alignment().raw_value()};
            const auto alignedWidth{excess ? width() - excess + row_alignment().raw_value() : width()};
            return alignedWidth * num_channels().raw_value();
        }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Height; }

        [[nodiscard]]
        image_channels num_channels() const noexcept { return m_Channels; }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Alignment; }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return m_Data; }

        [[nodiscard]]
        friend bool operator==(const image_view&, const image_view&) noexcept = default;
    private:
        std::span<const value_type> m_Data;
        std::size_t m_Width{}, m_Height{};
        image_channels m_Channels;
        alignment m_Alignment{};
    };
}

namespace std {
    template<>
    struct formatter<avocet::image_channels> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(const avocet::image_channels& channels, auto& ctx) const {
            return std::format_to(ctx.out(), "{}", channels.raw_value());
        }
    };
}
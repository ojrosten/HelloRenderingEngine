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
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <variant>
#include <utility>

#include "sequoia/Core/Meta/Utilities.hpp"

namespace avocet {
    enum class flip_vertically : bool { no, yes };

    class colour_channels {
        std::size_t m_Value{};
    public:
        constexpr colour_channels() noexcept = default;

        constexpr explicit colour_channels(std::size_t n) noexcept
            : m_Value{n}
        {}

        [[nodiscard]]
        friend constexpr auto operator<=>(const colour_channels&, const colour_channels&) noexcept = default;

        [[nodiscard]]
        constexpr std::size_t raw_value() const noexcept { return m_Value; }
    };

    class alignment {
        std::size_t m_Value{1};

        [[nodiscard]]
        constexpr static std::size_t validate(std::size_t val) {
            const bool powOfTwo{(val > 0) && ((val & (val - 1)) == 0)};
            if(!powOfTwo)
                throw std::runtime_error{std::format("alignment: value {} is not a power of 2", val)};

            return val;
        }
    public:
        constexpr alignment() noexcept = default;

        constexpr explicit alignment(std::size_t n)
            : m_Value{validate(n)}
        {}

        [[nodiscard]]
        friend constexpr auto operator<=>(const alignment&, const alignment&) noexcept = default;

        [[nodiscard]]
        constexpr std::size_t raw_value() const noexcept { return m_Value; }
    };

    [[nodiscard]]
    constexpr std::size_t padded_row_size(std::size_t width, colour_channels channels, alignment rowAlignment, std::size_t bytesPerChannel) {
        constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};

        const auto bytesPerTexel{channels.raw_value() * bytesPerChannel};
        if(bytesPerTexel && (maxVal / bytesPerTexel < width))
            throw std::runtime_error{std::format("padded_row_size: width ({}) * channels ({}) * bytes per channel ({}) exceed the maximum allowed value {}", width, channels.raw_value(), bytesPerChannel, maxVal)};

        const std::size_t nominalRowSize{width * bytesPerTexel};

        if(const std::size_t overhangingBytes{nominalRowSize % rowAlignment.raw_value()}; overhangingBytes) {
            if(nominalRowSize - overhangingBytes > maxVal - rowAlignment.raw_value())
                throw std::runtime_error{std::format("padded_row_size: nominal row size ({}) aligned to a ({}) byte boundary will exceed the maxmimu allowed value {}", nominalRowSize, rowAlignment.raw_value(), maxVal)};

            return nominalRowSize - overhangingBytes + rowAlignment.raw_value();
        }

        return nominalRowSize;
    }

    void validate(std::size_t paddedRowSize, std::size_t height, std::size_t size);

    inline constexpr std::optional<colour_channels> all_channels_in_image{std::nullopt};

    namespace impl {
        template<sequoia::movable_comparable Width, sequoia::movable_comparable Height, sequoia::movable_comparable NumChannels, sequoia::movable_comparable Alignment>
        struct image_spec {
            Width width;
            Height height;
            NumChannels channels;
            Alignment row_alignment;

            [[nodiscard]]
            friend bool operator==(const image_spec&, const image_spec&) noexcept = default;
        };
    }

    class unique_image {
    public:
        using value_type = unsigned char;

        unique_image(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<colour_channels> requestedChannels)
            : unique_image{make(texturePath, flip, requestedChannels)}
        {}

        unique_image(std::vector<value_type> data, std::size_t imageWidth, std::size_t imageHeight, colour_channels channels, alignment rowAlignment)
            : m_Data{std::move(data)}
            , m_Spec{.width{imageWidth}, .height{imageHeight}, .channels{channels}, .row_alignment{rowAlignment}}
        {
            validate(padded_row_size(), height(), std::get<vec_t>(m_Data).size());
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Spec.width.value; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Spec.height.value; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.channels.value; }

        [[nodiscard]]
        std::size_t size() const noexcept { return height() * padded_row_size(); }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Spec.row_alignment.value; }

        [[nodiscard]]
        std::size_t padded_row_size() const noexcept {
            return avocet::padded_row_size(width(), num_channels(), row_alignment(), sizeof(value_type));
        }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept {
            using span_t = std::span<const value_type>;

            return std::visit(
                sequoia::overloaded{
                    [sz{size()}] (const ptr_t& p) { return span_t{p.get(), sz}; },
                              [] (const vec_t& v) { return span_t{v}; }
                },
                m_Data
            );
        }

        [[nodiscard]]
        friend bool operator==(const unique_image&, const unique_image&) noexcept = default;
    private:
        struct file_unloader {
            void operator()(value_type* ptr) const;
        };

        template<class T>
        struct parameter {
            [[nodiscard]]
            std::size_t to_unsigned(int val) {
                if(val < 0)
                    throw std::logic_error{std::format("unique_image::parameter - negative value {}", val)};

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
        impl::image_spec<parameter<std::size_t>, parameter<std::size_t>, parameter<colour_channels>, parameter<alignment>> m_Spec;

        unique_image(value_type* ptr, int width, int height, int channels, alignment rowAlignment)
            : m_Data{ptr_t{ptr}}
            , m_Spec{.width{width}, .height{height}, .channels{channels}, .row_alignment{rowAlignment}}
        {}

        [[nodiscard]]
        static unique_image make(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<colour_channels> requestedChannels);
    };

    class image_view {
    public:
        using value_type = unsigned char;

        image_view(std::span<const value_type> data, std::size_t imageWidth, std::size_t imageHeight, colour_channels numChannels, alignment rowAlignment)
            : m_Data{data}
            , m_Spec{.width{imageWidth}, .height{imageHeight}, .channels{numChannels}, .row_alignment{rowAlignment}}
        {
            validate(height(), padded_row_size(), m_Data.size());
        }

        image_view(const unique_image& im)
            : m_Data{im.span()}
            , m_Spec{.width{im.width()}, .height{im.height()}, .channels{im.num_channels()}, .row_alignment{im.row_alignment()}}
        {}

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Spec.width; }

        [[nodiscard]]
        std::size_t padded_row_size() const noexcept { 
            return avocet::padded_row_size(width(), num_channels(), row_alignment(), sizeof(value_type));
        }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Spec.height; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.channels; }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Spec.row_alignment; }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return m_Data; }

        [[nodiscard]]
        friend bool operator==(const image_view& lhs, const image_view& rhs) noexcept {
            return std::ranges::equal(lhs.span(), rhs.span()) && lhs.m_Spec == rhs.m_Spec;
        }
    private:
        std::span<const value_type> m_Data;
        impl::image_spec<std::size_t, std::size_t, colour_channels, alignment> m_Spec;
    };
}

namespace std {
    template<>
    struct formatter<avocet::colour_channels> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(const avocet::colour_channels& channels, auto& ctx) const {
            return std::format_to(ctx.out(), "{}", channels.raw_value());
        }
    };

    template<>
    struct formatter<avocet::alignment> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::alignment a, auto& ctx) const {
            return std::format_to(ctx.out(), "{}", a.raw_value());
        }
    };
}

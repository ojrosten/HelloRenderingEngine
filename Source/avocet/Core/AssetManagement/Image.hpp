////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/RenderArea/Viewport.hpp"

#include "sequoia/Core/Meta/Utilities.hpp"

#include <filesystem>
#include <format>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace avocet {
    enum class flip_vertically : bool { no, yes };

    class colour_channels {
        std::size_t m_Value{};
    public:
        constexpr colour_channels() noexcept = default;

        constexpr explicit colour_channels(std::size_t val) noexcept : m_Value{val} {}

        [[nodiscard]]
        friend constexpr auto operator<=>(const colour_channels&, const colour_channels&) noexcept = default;

        [[nodiscard]]
        constexpr std::size_t raw_value() const noexcept { return m_Value; }
    };

    inline constexpr std::optional<colour_channels> all_channels_in_image{std::nullopt};

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

        constexpr explicit alignment(std::size_t val) : m_Value{validate(val)}
        {}

        [[nodiscard]]
        friend constexpr auto operator<=>(const alignment&, const alignment&) noexcept = default;

        [[nodiscard]]
        constexpr std::size_t raw_value() const noexcept { return m_Value; }
    };

    [[nodiscard]]
    std::size_t padded_row_size(std::size_t width, colour_channels channels, std::size_t bytesPerChannel, alignment rowAlignment);

    [[nodiscard]]
    std::size_t safe_image_size(std::size_t width, std::size_t height);

    namespace impl {
        template<class ImageValueType>
        struct image_spec {
            using value_type = ImageValueType;

            std::size_t     width;
            std::size_t     height;
            colour_channels channels;
            alignment       row_alignment;

            [[nodiscard]]
            friend bool operator==(const image_spec&, const image_spec&) noexcept = default;

            [[nodiscard]]
            std::size_t padded_row_size() const {
                return avocet::padded_row_size(width, channels, sizeof(value_type), row_alignment);
            }

            [[nodiscard]]
            std::size_t size() const { return safe_image_size(height, padded_row_size()); }

            void validate(std::size_t imageSize) const {
                if(imageSize != size())
                    throw std::runtime_error{std::format("unique_image size {} is not height ({}) * padded_row_size ({})\n[width = {}; channels = {}; alignment = {}]", imageSize, height, padded_row_size(), width, channels, row_alignment)};
            }
        };
    }

    class unique_image {
    public:
        using value_type = unsigned char;

        unique_image(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<colour_channels> requestedChannels)
            : unique_image{make(texturePath, flip, requestedChannels)}
        {}

        unique_image(std::vector<value_type> data, std::size_t width, std::size_t height, colour_channels channels, alignment rowAlignment)
            : m_Data{std::move(data)}
            , m_Spec{{.width{width}, .height{height}, .channels{channels}, .row_alignment{rowAlignment}}}
        {
            m_Spec.value.validate(std::get<vec_t>(m_Data).size());
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Spec.value.width; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Spec.value.height; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.value.channels; }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Spec.value.row_alignment; }

        [[nodiscard]]
        std::size_t padded_row_size() const { return m_Spec.value.padded_row_size(); }

        [[nodiscard]]
        std::size_t size() const { return m_Spec.value.size(); }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept {
            using span_t = std::span<const value_type>;
            return std::visit(
                sequoia::overloaded{
                    [sz{size()}](const ptr_t& ptr) { return span_t{ptr.get(), sz}; },
                              [](const vec_t& vec) { return span_t{vec}; }
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

        using spec_t = impl::image_spec<value_type>;

        struct specification {
            spec_t value{};

            explicit specification(const spec_t& val) : value{val} {}

            specification(specification&& other) noexcept 
                : value{std::exchange(other.value, spec_t{})}
            {}

            specification& operator=(specification&& other) noexcept {
                if(this != &other)
                    value = std::exchange(other.value, spec_t{});

                return *this;
            }

            [[nodiscard]]
            bool operator==(const specification&) const noexcept = default;
        };

        using ptr_t = std::unique_ptr<value_type, file_unloader>;
        using vec_t = std::vector<value_type>;

        std::variant<ptr_t, vec_t> m_Data;
        specification m_Spec;

        unique_image(value_type* ptr, int width, int height, int channels, alignment rowAlignment)
            : m_Data{ptr_t{ptr}}
            , m_Spec{spec_t{.width{to_unsigned(width)}, .height{to_unsigned(height)}, .channels{to_unsigned(channels)}, .row_alignment{rowAlignment}}}
        {}

        [[nodiscard]]
        static unique_image make(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<colour_channels> requestedChannels);

        [[nodiscard]]
        static std::size_t to_unsigned(int val) {
            if(val < 0)
                throw std::logic_error{std::format("unique_image::specification - negative value {}", val)};

            return static_cast<std::size_t>(val);
        }
    };

    class image_view {
    public:
        using value_type = unique_image::value_type;

        image_view(const unique_image& image)
            : m_Span{image.span()}
            , m_Spec{.width{image.width()}, .height{image.height()}, .channels{image.num_channels()}, .row_alignment{image.row_alignment()}}
        {}

        image_view(std::span<const value_type> data, std::size_t width, std::size_t height, colour_channels channels, alignment rowAlignment)
            : m_Span{data}
            , m_Spec{.width{width}, .height{height}, .channels{channels}, .row_alignment{rowAlignment}}
        {
            m_Spec.validate(m_Span.size());
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Spec.width; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Spec.height; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.channels; }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Spec.row_alignment; }

        [[nodiscard]]
        std::size_t padded_row_size() const { return m_Spec.padded_row_size(); }

        [[nodiscard]]
        std::size_t size() const { return m_Spec.size(); }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return m_Span; }

        [[nodiscard]]
        friend bool operator==(const image_view& lhs, const image_view& rhs) noexcept {
            return (lhs.m_Spec == rhs.m_Spec) && std::ranges::equal(lhs.span(), rhs.span());
        }
    private:
        std::span<const value_type> m_Span;
        impl::image_spec<value_type> m_Spec;
    };
}

namespace std {
    template<>
    struct formatter<avocet::colour_channels> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::colour_channels channels, auto& ctx) const {
            return format_to(ctx.out(), "{}", channels.raw_value());
        }
    };

    template<>
    struct formatter<avocet::alignment> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::alignment a, auto& ctx) const {
            return format_to(ctx.out(), "{}", a.raw_value());
        }
    };
}

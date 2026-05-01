////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Geometry/Extent.hpp"
#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

#include "sequoia/Core/Meta/Utilities.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <memory>

#include <ranges>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace avocet {
    enum class flip_vertically : bool { no, yes };

    class colour_channels {
        std::uint32_t m_Value{};
    public:
        constexpr colour_channels() noexcept = default;

        constexpr explicit colour_channels(std::uint32_t val) noexcept : m_Value{val} {}

        [[nodiscard]]
        friend constexpr auto operator<=>(const colour_channels&, const colour_channels&) noexcept = default;

        [[nodiscard]]
        constexpr std::uint32_t raw_value() const noexcept { return m_Value; }
    };

    inline constexpr std::optional<colour_channels> all_channels_in_image{std::nullopt};

    class alignment {
        std::uint32_t m_Value{1};

        [[nodiscard]]
        constexpr static std::uint32_t validate(std::uint32_t val) {
            const bool powOfTwo{(val > 0) && ((val & (val - 1)) == 0)};
            if(!powOfTwo)
                throw std::runtime_error{std::format("alignment: value {} is not a power of 2", val)};

            return val;
        }
    public:
        constexpr alignment() noexcept = default;

        constexpr explicit alignment(std::uint32_t val) : m_Value{validate(val)}
        {}

        [[nodiscard]]
        friend constexpr auto operator<=>(const alignment&, const alignment&) noexcept = default;

        [[nodiscard]]
        constexpr std::uint32_t raw_value() const noexcept { return m_Value; }
    };

    [[nodiscard]]
    uint32_t padded_row_size(uint32_t width, colour_channels channels, std::uint32_t bytesPerChannel, alignment rowAlignment);

    namespace impl {
        template<class ImageValueType>
        struct image_spec {
            using value_type = ImageValueType;

            discrete_extent extent;
            colour_channels channels;
            alignment       row_alignment;

            [[nodiscard]]
            friend bool operator==(const image_spec&, const image_spec&) noexcept = default;

            [[nodiscard]]
            discrete_extent padded_extent() const {
                return {avocet::padded_row_size(extent.width, channels, sizeof(value_type), row_alignment), extent.height};
            }

            void validate(std::uint64_t imageSize) const {
                if (const auto paddedExtent{padded_extent()}; imageSize != paddedExtent.size())
                    throw std::runtime_error{std::format("unique_image size {} is not height ({}) * padded_row_size ({})\n[width = {}; channels = {}; alignment = {}]", imageSize, extent.height, paddedExtent.width, extent.width, channels, row_alignment)};
            }
        };
    }

    class unique_image {
    public:
        using value_type = unsigned char;

        unique_image(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<colour_channels> requestedChannels)
            : unique_image{make(texturePath, flip, requestedChannels)}
        {}

        unique_image(std::vector<value_type> data, discrete_extent extent, colour_channels channels, alignment rowAlignment)
            : m_Data{std::move(data)}
            , m_Spec{{.extent{extent},.channels{channels}, .row_alignment{rowAlignment}}}
        {
            m_Spec.value.validate(std::get<vec_t>(m_Data).size());
        }

        unique_image(std::span<const value_type> data, discrete_extent extent, colour_channels channels, alignment rowAlignment)
            : unique_image{std::ranges::to<std::vector>(data), extent, channels, rowAlignment}
        {}

        [[nodiscard]]
        discrete_extent extent() const noexcept { return m_Spec.value.extent; }

        [[nodiscard]]
        discrete_extent padded_extent() const noexcept { return m_Spec.value.padded_extent(); }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.value.channels; }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Spec.value.row_alignment; }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept {
            using span_t = std::span<const value_type>;
            return std::visit(
                sequoia::overloaded{
                    [sz{padded_extent().size()}](const ptr_t& ptr) { return span_t{ptr.get(), sz}; },
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
            , m_Spec{
                  spec_t{
                      .extent{  convert_value_to<std::uint32_t>(width),
                                convert_value_to<std::uint32_t>(height)
                       },
                      .channels{convert_value_to<std::uint32_t>(channels)},
                      .row_alignment{rowAlignment}
                  }
              }
        {}

        [[nodiscard]]
        static unique_image make(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<colour_channels> requestedChannels);
    };

    class image_view {
    public:
        using value_type = unique_image::value_type;

        image_view(const unique_image& image)
            : m_Span{image.span()}
            , m_Spec{.extent{image.extent()}, .channels{image.num_channels()}, .row_alignment{image.row_alignment()}}
        {}

        image_view(std::span<const value_type> data, discrete_extent extent, colour_channels channels, alignment rowAlignment)
            : m_Span{data}
            , m_Spec{.extent{extent}, .channels{channels}, .row_alignment{rowAlignment}}
        {
            m_Spec.validate(m_Span.size());
        }

        [[nodiscard]]
        discrete_extent extent() const noexcept { return m_Spec.extent; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.channels; }

        [[nodiscard]]
        alignment row_alignment() const noexcept { return m_Spec.row_alignment; }

        [[nodiscard]]
        discrete_extent padded_extent() const { return m_Spec.padded_extent(); }

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

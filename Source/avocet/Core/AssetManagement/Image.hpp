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
#include <utility>
#include <variant>

#include "sequoia/Core/Meta/Utilities.hpp"

namespace avocet {
    enum class flip_vertically : bool { no, yes };

    class colour_channels {
        std::size_t m_Value{};
    public:
        colour_channels() noexcept = default;

        explicit colour_channels(std::size_t val) noexcept : m_Value{val} {}

        [[nodiscard]]
        friend auto operator<=>(const colour_channels&, const colour_channels&) noexcept = default;

        [[nodiscard]]
        std::size_t raw_value() const noexcept { return m_Value; }
    };

    class alignment {
        std::size_t m_Value{1};

        [[nodiscard]]
        static std::size_t validate(std::size_t val) {
            const bool powOfTwo{(val > 0) && ((val & (val - 1)) == 0)};
            if(!powOfTwo)
                throw std::runtime_error{std::format("alignment: value {} is not a power of 2", val)};

            return val;
        }
    public:
        alignment() noexcept = default;

        explicit alignment(std::size_t val) : m_Value{validate(val)}
        {}

        [[nodiscard]]
        friend auto operator<=>(const alignment&, const alignment&) noexcept = default;

        [[nodiscard]]
        std::size_t raw_value() const noexcept { return m_Value; }
    };

    namespace impl {
        template<sequoia::movable_comparable Width, sequoia::movable_comparable Height, sequoia::movable_comparable Channels>
        struct image_spec {
            Width width;
            Height height;
            Channels channels;

            [[nodiscard]]
            friend bool operator==(const image_spec&, const image_spec&) noexcept = default;
        };
    }

    class unique_image {
    public:
        using value_type = unsigned char;

        unique_image(const std::filesystem::path& texturePath, flip_vertically flip)
            : unique_image{make(texturePath, flip)}
        {}

        unique_image(std::vector<value_type> data, std::size_t width, std::size_t height, colour_channels channels)
            : m_Data{std::move(data)}
            , m_Spec{.width{width}, .height{height}, .channels{channels}}
        {
            if(const auto sz{std::get<vec_t>(m_Data).size()}; sz != size())
                throw std::runtime_error{std::format("unique_image size {} is not width ({}) * height ({}) * channels ({})", sz, this->width(), this->height(), num_channels().raw_value())};
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Spec.width.value; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Spec.height.value; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.channels.value; }

        [[nodiscard]]
        std::size_t size() const noexcept { return width() * height() * num_channels().raw_value(); }

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

        template<sequoia::movable_comparable T>
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

            explicit parameter(T val) : value{val} {}

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
        impl::image_spec<parameter<std::size_t>, parameter<std::size_t>, parameter<colour_channels>> m_Spec;

        unique_image(value_type* ptr, int width, int height, int channels)
            : m_Data{ptr_t{ptr}}
            , m_Spec{.width{width}, .height{height}, .channels{channels}}
        {}

        [[nodiscard]]
        static unique_image make(const std::filesystem::path& texturePath, flip_vertically flip);
    };

    class image_view {
    public:
        using value_type = unique_image::value_type;

        image_view(const unique_image& image)
            : m_Span{image.span()}
            , m_Spec{.width{image.width()},
                     .height{image.height()},
                     .channels{image.num_channels()}}
        {}

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Spec.width; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Spec.height; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Spec.channels; }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return m_Span; }

        [[nodiscard]]
        friend bool operator==(const image_view& lhs, const image_view& rhs) noexcept {
            return (lhs.m_Spec == rhs.m_Spec) && std::ranges::equal(lhs.span(), rhs.span());
        }
    private:
        std::span<const value_type> m_Span;
        impl::image_spec<std::size_t, std::size_t, colour_channels> m_Spec;
    };
}

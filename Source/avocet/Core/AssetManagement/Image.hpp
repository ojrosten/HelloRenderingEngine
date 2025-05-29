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
#include <variant>

#include "sequoia/Core/Meta/Utilities.hpp"

namespace avocet {
    enum class flip_vertically : bool { no, yes };

    class colour_channels {
        std::size_t m_Value{};
    public:
        colour_channels() noexcept = default;

        explicit colour_channels(std::size_t val) : m_Value{val} {}

        [[nodiscard]]
        std::size_t raw_value() const noexcept { return m_Value; }

        [[nodiscard]]
        friend auto operator<=>(const colour_channels&, const colour_channels&) noexcept = default;
    };

    class unique_image {
    public:
        using value_type = unsigned char;

        unique_image(const std::filesystem::path& texturePath, flip_vertically flip)
            : unique_image{make(texturePath, flip)}
        {}

        unique_image(std::vector<value_type> data, std::size_t width, std::size_t height, colour_channels numChannels)
            : m_Data{std::move(data)}
            , m_Width{width}
            , m_Height{height}
            , m_Channels{numChannels}
        {
            if(const auto sz{std::get<vec_t>(m_Data).size()}; size() != sz)
                throw std::runtime_error{std::format("unique_image size {} != width {} * height {} * channels {}", sz, this->width(), this->height(), num_channels().raw_value())};
        }

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Width.value; }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Height.value; }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Channels.value; }

        [[nodiscard]]
        std::size_t size() const noexcept { return width() * height() * num_channels().raw_value(); }

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
            {}

            explicit parameter(T val)
                : value{val}
            {}

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
        parameter<colour_channels> m_Channels;

        unique_image(value_type* ptr, int width, int height, int channels)
            : m_Data{ptr_t{ptr}}
            , m_Width{width}
            , m_Height{height}
            , m_Channels{channels}
        {}

        [[nodiscard]]
        static unique_image make(const std::filesystem::path& texturePath, flip_vertically flip);
    };

    class image_view {
    public:
        using value_type = unique_image::value_type;

        image_view(const unique_image& image)
            : m_Image{&image}
        {}

        [[nodiscard]]
        std::size_t width() const noexcept { return m_Image->width(); }

        [[nodiscard]]
        std::size_t height() const noexcept { return m_Image->height(); }

        [[nodiscard]]
        colour_channels num_channels() const noexcept { return m_Image->num_channels(); }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return m_Image->span(); }

        [[nodiscard]]
        friend bool operator==(const image_view& lhs, const image_view& rhs) noexcept {
            return std::ranges::equal(lhs.span(), rhs.span());
        }
    private:
        const unique_image* m_Image;
    };
}

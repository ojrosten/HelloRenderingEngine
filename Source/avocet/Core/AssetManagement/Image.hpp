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

namespace avocet {
    enum class flip_vertically : bool { no, yes };

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
        std::size_t num_channels() const noexcept { return m_Channels.value; }

        [[nodiscard]]
        std::size_t size() const noexcept { return width() * height() * num_channels(); }

        [[nodiscard]]
        std::span<value_type> span() noexcept { return {m_Data.get(), size()}; }

        [[nodiscard]]
        std::span<const value_type> span() const noexcept { return {m_Data.get(), size()}; }

        [[nodiscard]]
        friend bool operator==(const image&, const image&) noexcept = default;
    private:
        struct file_unloader {
            void operator()(value_type* ptr) const;
        };

        struct parameter {
            [[nodiscard]]
            std::size_t to_unsigned(int val) {
                if(val < 0)
                    throw std::logic_error{std::format("image::parameter - negative value {}", val)};

                return static_cast<std::size_t>(val);
            }

            std::size_t value{};

            explicit parameter(int val)
                : value{to_unsigned(val)}
            { }

            parameter(parameter&& other) noexcept 
                : value{std::exchange(other.value, 0)}
            {}

            parameter& operator=(parameter&& other) noexcept {
                if(this != &other)
                    value = std::exchange(other.value, 0);

                return *this;
            }

            [[nodiscard]]
            auto operator<=>(const parameter&) const noexcept = default;
        };

        std::unique_ptr<value_type, file_unloader> m_Data;
        parameter m_Width, m_Height, m_Channels;

        image(value_type* ptr, int width, int height, int channels)
            : m_Data{ptr}
            , m_Width{width}
            , m_Height{height}
            , m_Channels{channels}
        {}

        [[nodiscard]]
        static image make(const std::filesystem::path& texturePath, flip_vertically flip);
    };
}

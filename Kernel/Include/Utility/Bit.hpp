#pragma once

#include "BaseType.hpp"

namespace Kernel
{
    /// @brief 値のビットが立っているかを調べる
    /// @param value 調べる値
    /// @param bit 調べるビット
    /// @return 0ならfalse,1ならtrue
    inline bool Bit(std::uint8_t value, int bit) noexcept
    {
        if (bit >= CHAR_BIT)
        {
            return false;
        }

        return static_cast<bool>((value >> bit) & 1);
    }

    inline void SetBit(std::uint8_t& value, int bit, bool flag) noexcept
    {
        if (bit >= CHAR_BIT)
        {
            return;
        }

        const std::uint8_t mask = static_cast<std::uint8_t>(1 << bit);
        value = static_cast<std::uint8_t>((value & static_cast<std::uint8_t>(~mask)) | (flag ? mask : static_cast<std::uint8_t>(0)));
    }

    inline void SetBitRange(std::uint8_t* data, std::size_t size, std::size_t startIndex, int startBit, std::size_t length, bool flag) noexcept
    {
        if (length == 0)
        {
            return;
        }

        const std::size_t total = size * CHAR_BIT;
        const std::size_t sb = startIndex * CHAR_BIT + static_cast<std::size_t>(startBit);

        if (sb >= total)
        {
            return;
        }

        if (sb + length > total)
        {
            return;
        }

        for (std::size_t i = 0; i < length; ++i)
        {
            const std::size_t b = sb + i;
            const std::size_t idx = b / CHAR_BIT;
            const int bit = static_cast<int>(b % CHAR_BIT);
            SetBit(data[idx], bit, flag);
        }
    }
}
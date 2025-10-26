#pragma once

#include "BaseType.hpp"

namespace Kernel
{
    struct InterfaceID
    {
        std::uint8_t ID[16];
        constexpr bool operator==(const InterfaceID& rhs) const noexcept
        {
            for (int i = 0; i < 16; ++i)
            {
                if (ID[i] != rhs.ID[i])
                {
                    return false;
                }
            }

            return true;
        }

        constexpr bool operator!=(const InterfaceID& rhs) const noexcept
        {
            return !(*this == rhs);
        }
    };
}
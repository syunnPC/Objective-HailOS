#pragma once

#include "InterfaceID.hpp"

namespace Kernel
{
    inline constexpr InterfaceID IID_IStringConvertable{ {
        0x1D, 0x24, 0x5F, 0xCD, 0x93, 0xCA, 0x2C, 0x1E, 0x79, 0x55, 0x35, 0xCB, 0xAE, 0xBD, 0xC2, 0x5C
    } };

    class IStringConvertible
    {
    public:
        virtual std::size_t ToString(char* buf, std::size_t size) noexcept = 0;
    protected:
        virtual ~IStringConvertible() = default;
    };
}
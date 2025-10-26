#pragma once

#include "IDevice.hpp"

namespace Kernel
{
    inline constexpr InterfaceID IID_IRandomDevice{ {
        0x2B, 0xCB, 0x13, 0xFD, 0x10, 0x75, 0xBC, 0xA8, 0xE0, 0x9D, 0xFD, 0x28, 0xF1, 0x82, 0x21, 0xF6
    } };

    class IRandomDevice : public virtual IDevice
    {
    public:
        virtual std::uint16_t Generate16() noexcept = 0;
        virtual std::uint32_t Generate32() noexcept = 0;
        virtual std::uint64_t Generate64() noexcept = 0;
    protected:
        virtual ~IRandomDevice() override = default;
    };
}
#pragma once

#include "IDevice.hpp"

namespace Kernel
{
    inline constexpr InterfaceID IID_IOutputDevice{ {
        0x0C, 0x55, 0x02, 0x96, 0xF9, 0xB9, 0x2A, 0x42, 0x3C, 0x74, 0x63, 0xAE, 0x16, 0x57, 0x9D, 0x25
    } };

    class IOutputDevice : public virtual IDevice
    {
    public:
        virtual void PutChar(char ch) noexcept = 0;
        virtual void PutString(const char* str) noexcept = 0;
        virtual void Flush() noexcept = 0;
    protected:
        virtual ~IOutputDevice() override = default;
    };
}
#pragma once

#include "IDevice.hpp"

namespace Kernel
{
    inline constexpr InterfaceID IID_IDiskDrive{ {
        0x0C, 0x6C, 0xFB, 0xE8, 0x0C, 0x95, 0xE0, 0x37, 0xB1, 0xA3, 0xE3, 0xE4, 0xCE, 0x84, 0xFD, 0x5F
    } };

    class IDiskDrive : public virtual IDevice
    {
    public:
        virtual ::Status Read(std::uint64_t lba, std::size_t count, void* buffer) noexcept = 0;
        virtual ::Status Write(std::uint64_t lba, std::size_t count, const void* buffer) noexcept = 0;
    protected:
        virtual ~IDiskDrive() override = default;
    };
}
#pragma once

#include "IO.hpp"

namespace
{
    constexpr auto PIC_EOI = 0x20;
    constexpr auto PIC_MASTER_CMD = 0x20;
    constexpr auto PIC_MASTER_DATA = 0x21;
    constexpr auto PIC_SLAVE_CMD = 0xA0;
    constexpr auto PIC_SLAVE_DATA = 0xA1;
    constexpr auto ICW1_INIT = 0x10;
    constexpr auto ICW1_ICW4 = 0x01;
    constexpr auto ICW4_8086 = 0x01;
    constexpr auto PIC_MASTER_ISR_OFFSET = 0x20;
    constexpr auto PIC_SLAVE_ISR_OFFSET = 0x28;
}

namespace Kernel::Arch::x86_64::PIC
{
    inline void EOIMaster() noexcept
    {
        Kernel::Arch::x86_64::IO::Out8(0x20, 0x20);
    }

    inline void EOISlave() noexcept
    {
        Kernel::Arch::x86_64::IO::Out8(0xA0, 0x20);
        Kernel::Arch::x86_64::IO::Out8(0x20, 0x20);
    }

    inline bool IsMasterVector(std::uint8_t vec) noexcept
    {
        return vec >= 0x20 && vec <= 0x27;
    }

    inline bool IsSlaveVector(std::uint8_t vec) noexcept
    {
        return vec >= 0x28 && vec <= 0x2F;
    }

    inline void Unmask(std::uint8_t irq)
    {
        std::uint16_t port;
        std::uint8_t value;

        if (irq < 8)
        {
            port = PIC_MASTER_DATA;
        }
        else
        {
            port = PIC_SLAVE_DATA;
            irq -= 8;
        }

        value = IO::In8(port) & ~(1 << irq);
        IO::Out8(port, value);
    }

    inline void Mask(std::uint8_t irq)
    {
        std::uint16_t port;
        std::uint8_t value;

        if (irq < 8)
        {
            port = PIC_MASTER_DATA;
        }
        else
        {
            port = PIC_SLAVE_DATA;
            irq -= 8;
        }

        value = IO::In8(port) | (1 << irq);
        IO::Out8(port, value);
    }
}
#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64::IO
{
    inline void Out8(std::uint16_t port, std::uint8_t value)
    {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
    }

    inline void Out16(std::uint16_t port, std::uint16_t value)
    {
        asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
    }

    inline void Out32(std::uint16_t port, std::uint32_t value)
    {
        asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
    }

    inline std::uint8_t In8(std::uint16_t port)
    {
        std::uint8_t result;
        asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    inline std::uint16_t In16(std::uint16_t port)
    {
        std::uint16_t result;
        asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    inline std::uint32_t In32(std::uint16_t port)
    {
        std::uint32_t result;
        asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }
}
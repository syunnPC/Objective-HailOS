#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    struct GDTR
    {
        std::uint16_t Limit;
        std::uintptr_t Base;
    } __attribute__((packed));

    struct GDTEntry
    {
        std::uint16_t LimitLow;
        std::uint16_t BaseLow;
        std::uint8_t BaseMid;
        std::uint8_t Access;
        std::uint8_t Granularity;
        std::uint8_t BaseHigh;
    } __attribute__((packed));

    constexpr std::uint16_t GDT_NULL_SEL = 0x00;
    constexpr std::uint16_t GDT_KERNEL_CS = 0x08;
    constexpr std::uint16_t GDT_KERNEL_DS = 0x10;
    constexpr std::uint16_t GDT_USER_CS = 0x18;
    constexpr std::uint16_t GDT_USER_DS = 0x20;
    constexpr std::uint16_t GDT_TSS_SEL = 0x28;
}
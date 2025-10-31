#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    struct IDTR
    {
        std::uint16_t Limit;
        std::uintptr_t Base;
    } __attribute__((packed));

    struct IDTEntry
    {
        std::uint16_t OffsetLow;
        std::uint16_t Selector;
        std::uint8_t Ist;
        std::uint8_t TypeAttribute;
        std::uint16_t OffsetMid;
        std::uint32_t OffsetHigh;
        std::uint32_t Reserved;
    } __attribute__((packed));

    constexpr auto IDT_ENTRIES = 256;
    constexpr std::uint8_t IDT_TYPE_INTERRUPT_GATE = 0x8E;
    constexpr std::uint8_t IDT_TYPE_TRAP_GATE = 0x8F;
}
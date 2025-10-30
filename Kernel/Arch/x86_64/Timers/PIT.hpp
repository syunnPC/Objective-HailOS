#pragma once

#include "BaseType.hpp"
#include "IO.hpp"
#include "PIC.hpp"

namespace Kernel::Arch::x86_64::Timers::PIT
{
    [[deprecated]] inline void InitPIT(std::uint32_t targetHz) noexcept
    {
        std::uint16_t div = 1193182u / targetHz;
        IO::Out8(0x43, 0x34);
        IO::Out8(0x40, static_cast<std::uint8_t>(div & 0xFF));
        IO::Out8(0x40, static_cast<std::uint8_t>((div >> 8) & 0xFF));

        PIC::Unmask(0);
    }
}
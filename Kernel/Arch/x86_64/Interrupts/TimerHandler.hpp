#pragma once

namespace Kernel::Arch::x86_64::Interrupts
{
    void InitTimerHandler(std::uint8_t vec) noexcept;
}
#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64::Interrupts
{
    void InitIRQ0Handler(std::uint8_t vec) noexcept;
}
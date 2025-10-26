#pragma once

#include "LAPIC.hpp"

namespace Kernel::Arch::x86_64::APIC
{
    constexpr std::uint8_t VEC_SPURIOUS = 0xFF;
    constexpr std::uint8_t VEC_ERROR = 0xF1;

    inline bool InitializeAPIC() noexcept
    {
        if (!IsPresent())
        {
            return false;
        }

        Enable();
        InitializeEarly(VEC_SPURIOUS, VEC_ERROR);

        DisableLegacyPIC();

        return true;
    }
}
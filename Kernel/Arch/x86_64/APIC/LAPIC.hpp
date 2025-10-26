#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64::APIC
{
    bool IsPresent() noexcept;
    void Enable() noexcept;
    void InitializeEarly(std::uint8_t spuriousVector, std::uint8_t errorVector) noexcept;
    void EndOfInterrupt() noexcept;
    std::uint32_t ReadLAPICID() noexcept;
    std::uint32_t ReadLAPICVersion() noexcept;
    void DisableLegacyPIC() noexcept;
}
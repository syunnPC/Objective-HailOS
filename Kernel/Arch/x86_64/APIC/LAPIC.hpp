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

    //LAPICタイマ
    void ConfigureTimer(std::uint8_t vector, std::uint8_t divideShift, std::uint32_t initialCount, bool periodic) noexcept;
    void MaskTimer(bool masked) noexcept;
    std::uint32_t CalibrateTimerInitialCountTSC(std::uint64_t tscHz, std::uint8_t divideShift, std::uint32_t targetPeriodUs, std::uint32_t sampleMs = 50) noexcept;
}
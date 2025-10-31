#pragma once

#include "BaseType.hpp"
#include "MSR.hpp"

namespace Kernel::Arch::x86_64
{
    struct PerCPU
    {
        std::uint32_t LAPICID{ 0 };
        std::uint32_t CPUIndex{ 0 };
        std::uint32_t PreemptDepth{ 0 };
        volatile std::uint64_t TickCount{ 0 };
        void* CurrentThread{ nullptr };
    };

    inline PerCPU& CPU() noexcept
    {
        auto base = Kernel::Arch::x86_64::MSR::Read(Kernel::Arch::x86_64::MSR::IA32_GS_BASE);
        return *reinterpret_cast<PerCPU*>(base);
    }

    void SetPerCPUBase(PerCPU* base) noexcept;
    void InitPerCPU() noexcept;
}
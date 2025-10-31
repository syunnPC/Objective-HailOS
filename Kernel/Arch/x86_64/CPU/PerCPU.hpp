#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    struct PerCPU
    {
        std::uint32_t LAPICID{ 0 };
        std::uint32_t CPUIndex{ 0 };
        std::uint32_t PreemptDepth{ 0 };
        std::uint64_t TickCount{ 0 };
        void* CurrentThread{ nullptr };
    };

    inline PerCPU& CPU() noexcept
    {
        PerCPU* p;
        asm volatile("mov %%gs:0, %0" : "=r"(p));
        return *p;
    }

    void SetPerCPUBase(PerCPU* base) noexcept;
    void InitPerCPU() noexcept;
}
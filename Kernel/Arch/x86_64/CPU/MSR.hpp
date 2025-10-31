#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64::MSR
{
    inline constexpr std::uint32_t IA32_GS_BASE = 0xC0000101;
    inline constexpr std::uint32_t IA32_KERNEL_GS_BASE = 0xC0000102;

    inline void Write(std::uint32_t msr, std::uint64_t value) noexcept
    {
        std::uint32_t lo = static_cast<std::uint32_t>(value & 0xFFFFFFFFull);
        std::uint32_t hi = static_cast<std::uint32_t>(value >> 32);
        asm volatile("wrmsr" : : "c"(msr), "a"(lo), "d"(hi) : );
    }

    inline std::uint64_t Read(std::uint32_t msr) noexcept
    {
        std::uint32_t lo, hi;
        asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr) : );
        return ((static_cast<std::uint64_t>(hi) << 32) | lo);
    }
}
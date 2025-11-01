#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    inline void CPUID(std::uint32_t leaf, std::uint32_t subleaf, std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d) noexcept
    {
        asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(leaf), "c"(subleaf));
    }
}
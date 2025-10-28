#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    inline std::uint64_t ReadTSC()
    {
        std::uint32_t lo;
        std::uint32_t hi;
        asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
        return ((static_cast<std::uint64_t>(hi) << 32) | lo);
    }
}
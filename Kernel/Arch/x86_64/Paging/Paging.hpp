#pragma once

#include "MemoryInfo.hpp"
#include "IPageAllocator.hpp"

namespace Kernel::Arch::x86_64
{
    inline void FlushTLB()
    {
        asm volatile(
            "mov %%cr3, %%rax\n\t"
            "mov %%rax, %%cr3\n\t"
            : : : "rax", "memory"
            );
    }

    bool BuildIdentityMap2M(const MemoryInfo* info, std::uint64_t& outCR3Phys, IPageAllocator& allocator);
}
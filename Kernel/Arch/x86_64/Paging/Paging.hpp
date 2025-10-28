#pragma once

#include "MemoryInfo.hpp"
#include "IPageAllocator.hpp"

namespace Kernel::Arch::x86_64
{
    inline void LoadCR3(std::uintptr_t pml4Phys)
    {
        asm volatile("mov %0, %%cr3" : : "r"(pml4Phys) : "memory");
    }

    inline std::uint64_t ReadCR3()
    {
        std::uint64_t v;
        asm volatile("mov %%cr3, %0" : "=r"(v) :: "memory");
        return v;
    }

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
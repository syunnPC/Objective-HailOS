#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    void StartCriticalSection();
    void EndCriticalSection();
    void EnableInterrupt();
    void DisableInterrupt();

    inline std::uint64_t SaveRFLAGS() noexcept
    {
        std::uint64_t f;
        asm volatile("pushfq; pop %0" : "=r"(f) :: "memory");
        return f;
    }

    inline void InterruptRestore(std::uint64_t flags) noexcept
    {
        if (flags & (1ull << 9))
        {
            asm volatile("sti" ::: "memory");
        }
    }
}
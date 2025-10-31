#pragma once

#include "BaseType.hpp"

namespace Kernel::Sync
{
    struct IRQState
    {
        std::uint64_t RFlags;
    };

    inline IRQState IRQSaveAndDisable() noexcept
    {
        IRQState s{};
        asm volatile("pushfq; popq %0; cli" : "=r"(s.RFlags) :: "memory");
        return s;
    }

    inline void IRQRestore(IRQState s) noexcept
    {
        if (s.RFlags & (1ull << 9))
        {
            asm volatile("sti" ::: "memory");
        }
        else
        {
            asm volatile("" ::: "memory");
        }
    }

    struct TicketSpinLock
    {
        volatile std::uint32_t Next{ 0 };
        std::uint32_t Owner{ 0 };
    };

    inline void CPURelax() noexcept
    {
        asm volatile("pause");
    }

    inline void Lock(TicketSpinLock& l) noexcept
    {
        const std::uint32_t t = __atomic_fetch_add(&l.Next, 1u, __ATOMIC_ACQ_REL);

        while (true)
        {
            const std::uint32_t o = __atomic_load_n(&l.Owner, __ATOMIC_ACQUIRE);
            if (o == t)
            {
                break;
            }
            CPURelax();
        }
    }

    inline bool TryLock(TicketSpinLock& l) noexcept
    {
        std::uint32_t o = __atomic_load_n(&l.Owner, __ATOMIC_RELAXED);
        std::uint32_t n = __atomic_load_n(&l.Next, __ATOMIC_RELAXED);
        if (o != n)
        {
            return false;
        }

        return __atomic_compare_exchange_n(&l.Next, &n, n + 1u, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED);
    }

    inline void Unlock(TicketSpinLock& l) noexcept
    {
        __atomic_fetch_add(&l.Owner, 1u, __ATOMIC_RELEASE);
    }

    struct LockedWithIRQ
    {
        TicketSpinLock* LockPtr;
        IRQState State;
    };

    inline LockedWithIRQ LockIRQSave(TicketSpinLock& l) noexcept
    {
        auto st = IRQSaveAndDisable();
        Lock(l);
        return LockedWithIRQ(&l, st);
    }

    inline void UnlockIRQRestor(LockedWithIRQ ctx) noexcept
    {
        Unlock(*ctx.LockPtr);
        IRQRestore(ctx.State);
    }
}
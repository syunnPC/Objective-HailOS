#pragma once

#include "RunQueue.hpp"

namespace Kernel::Sched
{
    void Init() noexcept;
    void Start() noexcept;
    void EnqueueReady(Thread* t) noexcept;
    Thread* DequeueNext() noexcept;

    void OnTimerTick() noexcept;
    void SwitchTo(Thread* next) noexcept;

    Thread* CreateAndEnqueue(const char* name, ThreadEntry entry, void* arg, std::size_t stackSize = 0) noexcept;
    void IdleMain(void* arg) noexcept;

    void SetTimeSliceTicks(std::uint32_t ticks) noexcept;

    bool NeedReschedule() noexcept;
    void ClearReschedule() noexcept;

    void SleepInsert(Thread* t, std::uint64_t wakeTick);
}
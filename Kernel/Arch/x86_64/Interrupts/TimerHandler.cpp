#include "KernelConsole.hpp"
#include "LAPIC.hpp"
#include "InterruptDispatch.hpp"
#include "Scheduler.hpp"

namespace
{
    void OnTimer(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& frame, bool hasError, std::uint64_t errorCode) noexcept
    {
        Kernel::Arch::x86_64::CPU().TickCount++;
        Kernel::Sched::OnTimerTick();
    }
}

namespace Kernel::Arch::x86_64::Interrupts
{
    void InitTimerHandler(std::uint8_t vec) noexcept
    {
        RegisterInterruptHandler(vec, &OnTimer);
    }
}
#include "KernelConsole.hpp"
#include "LAPIC.hpp"
#include "InterruptDispatch.hpp"

namespace
{
    void OnTimer(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& frame, bool hasError, std::uint64_t errorCode) noexcept
    {
        static volatile std::uint64_t tick = 0;
        ++tick;
        if ((tick & 1023ull) == 0)
        {
            if (auto con = Kernel::Early::GetKernelConsole())
            {
                con->PutString("[LAPIC Timer Tick]\n");
            }
        }
    }
}

namespace Kernel::Arch::x86_64::Interrupts
{
    void InitTimerHandler() noexcept
    {
        RegisterInterruptHandler(VEC_TIMER, &OnTimer);
    }
}
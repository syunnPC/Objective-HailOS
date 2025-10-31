#include "InterruptDispatch.hpp"
#include "APICController.hpp"
#include "KernelConsole.hpp"
#include "LAPIC.hpp"

namespace
{
    void OnIRQ0(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& frame, bool hasError, std::uint64_t errorCode) noexcept
    {
        static volatile std::uint64_t ctr = 0;
        ++ctr;
        if ((ctr & 1023ull) == 0)
        {
            if (auto con = Kernel::Early::GetBootstrapConsole())
            {
                con->PutString("[IOAPIC-IRQ0]\n");
            }
        }

        Kernel::Arch::x86_64::APIC::EndOfInterrupt();
    }
}

namespace Kernel::Arch::x86_64::Interrupts
{
    void InitIRQ0Handler(std::uint8_t vec) noexcept
    {
        RegisterInterruptHandler(vec, &OnIRQ0);
    }
}
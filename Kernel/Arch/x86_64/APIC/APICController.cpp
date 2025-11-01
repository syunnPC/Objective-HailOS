#include "APICController.hpp"
#include "APICInit.hpp"
#include "LAPIC.hpp"
#include "InterruptDispatch.hpp"

namespace Kernel::Arch::x86_64::APIC
{
    APICController::APICController() noexcept
    {
        static bool isInitialized = false;
        if (!isInitialized)
        {
            InitAPIC();
            isInitialized = true;
        }

        Interrupts::SetEOIEmitter(&EndOfInterrupt);
    }

    void APICController::Enable(std::uint8_t vector) noexcept
    {
        if (vector == Interrupts::VEC_TIMER)
        {
            MaskTimer(false);
            return;
        }
    }

    void APICController::Disable(std::uint8_t vector) noexcept
    {
        if (vector == Interrupts::VEC_TIMER)
        {
            MaskTimer(true);
            return;
        }
    }

    void APICController::EOI(std::uint8_t vector) noexcept
    {
        (void)vector;
        EndOfInterrupt();
    }
}
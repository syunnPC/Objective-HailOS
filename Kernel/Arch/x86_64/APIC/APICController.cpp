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

    void APICController::Enable(int vector) noexcept
    {

    }

    void APICController::Disable(int vector) noexcept
    {

    }

    void APICController::EOI(int vector) noexcept
    {
        (void)vector;
        EndOfInterrupt();
    }
}
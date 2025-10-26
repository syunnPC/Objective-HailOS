#include "APICController.hpp"
#include "APICInit.hpp"
#include "LAPIC.hpp"

namespace Kernel::Arch::x86_64::APIC
{
    APICController::APICController() noexcept
    {
        static bool isInitialized = false;
        if (!isInitialized)
        {
            InitializeAPIC();
            isInitialized = true;
        }
    }

    void APICController::Enable(int vector) noexcept
    {

    }

    void APICController::Disable(int vector) noexcept
    {

    }

    void APICController::EOI(int vector) noexcept
    {
        EndOfInterrupt();
    }
}
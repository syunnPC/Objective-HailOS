#pragma once

#include "IInterruptController.hpp"
#include "RefCountedBase.hpp"

namespace Kernel::Arch::x86_64::APIC
{
    class APICController : public virtual IInterruptController, public RefCountedBase
    {
    public:
        APICController() noexcept;
        APICController(const APICController&) = default;
        APICController(APICController&&) = default;
        virtual void Enable(int vector) noexcept override;
        virtual void Disable(int vector) noexcept override;
        virtual void EOI(int vector) noexcept override;
    protected:
        virtual ~APICController() noexcept = default;
    };
}
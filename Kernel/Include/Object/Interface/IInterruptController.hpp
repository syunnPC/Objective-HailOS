#pragma once

#include "IKernelObject.hpp"

namespace Kernel
{
    class IInterruptController : public virtual IKernelObject
    {
    public:
        virtual void Enable(std::uint8_t vector) noexcept = 0;
        virtual void Disable(std::uint8_t vector) noexcept = 0;
        virtual void EOI(std::uint8_t vector) noexcept = 0;
    protected:
        virtual ~IInterruptController() noexcept = default;
    };
}
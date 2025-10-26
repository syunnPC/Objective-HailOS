#pragma once

#include "IKernelObject.hpp"

namespace Kernel
{
    class IInterruptController : public virtual IKernelObject
    {
    public:
        virtual void Enable(int vector) noexcept = 0;
        virtual void Disable(int vector) noexcept = 0;
        virtual void EOI(int vector) noexcept = 0;
    protected:
        virtual ~IInterruptController() noexcept = default;
    };
}
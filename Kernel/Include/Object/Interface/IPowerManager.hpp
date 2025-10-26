#pragma once

#include "IKernelObject.hpp"

namespace Kernel
{
    class IPowerManager : public virtual IKernelObject
    {
    public:
        virtual void Shutdown() noexcept = 0;
    protected:
        virtual ~IPowerManager() noexcept = default;
    };
}
#pragma once

#include "PerCPU.hpp"

namespace Kernel::Arch::x86_64
{
    inline void PreemptDisable() noexcept
    {
        ++CPU().PreemptDepth;
    }

    inline void PreemptEnable() noexcept
    {
        auto& pc = CPU();
        if (pc.PreemptDepth > 0)
        {
            --pc.PreemptDepth;
        }
    }

    inline bool PreemptEnabled() noexcept
    {
        return CPU().PreemptDepth == 0;
    }

    class PreemptGuard
    {
    public:
        PreemptGuard() noexcept
        {
            PreemptDisable();
        }

        ~PreemptGuard() noexcept
        {
            PreemptEnable();
        }
    };
}
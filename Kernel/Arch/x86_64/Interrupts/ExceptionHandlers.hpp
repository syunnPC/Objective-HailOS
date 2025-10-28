#pragma once

#include "InterruptDispatch.hpp"

namespace Kernel::Arch::x86_64::Interrupts
{
    void InitCoreExceptionHandlers() noexcept;
}
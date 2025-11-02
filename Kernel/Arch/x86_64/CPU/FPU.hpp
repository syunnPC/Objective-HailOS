#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    extern "C" void FPUInitState(void* fx) noexcept;
    extern "C" void FPUSave(void* fx) noexcept;
    extern "C" void FPURestore(const void* fx) noexcept;
}
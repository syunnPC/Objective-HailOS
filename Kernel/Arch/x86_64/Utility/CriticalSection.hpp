#pragma once

namespace Kernel::Arch::x86_64
{
    void StartCriticalSection();
    void EndCriticalSection();
    void EnableInterrupt();
    void DisableInterrupt();
}
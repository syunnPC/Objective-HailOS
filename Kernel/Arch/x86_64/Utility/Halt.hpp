#pragma once

namespace Kernel::Arch::x86_64
{
    [[noreturn]] inline void HaltProcessor() noexcept
    {
        while (true)
        {
            asm volatile("cli");
            asm volatile("hlt");
        }
    }
}
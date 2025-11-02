#pragma once

#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    inline void WriteCR0(std::uint64_t value)
    {
        asm volatile("mov %0, %%cr0" : : "r"(value) : "memory");
    }

    inline std::uint64_t ReadCR0()
    {
        std::uint64_t v;
        asm volatile("mov %%cr0, %0" : "=r"(v) :: "memory");
        return v;
    }

    inline void WriteCR2(std::uint64_t value)
    {
        asm volatile("mov %0, %%cr2" : : "r"(value) : "memory");
    }

    inline std::uint64_t ReadCR2()
    {
        std::uint64_t v;
        asm volatile("mov %%cr2, %0" : "=r"(v) :: "memory");
        return v;
    }

    inline void WriteCR3(std::uint64_t value)
    {
        asm volatile("mov %0, %%cr3" : : "r"(value) : "memory");
    }

    inline std::uint64_t ReadCR3()
    {
        std::uint64_t v;
        asm volatile("mov %%cr3, %0" : "=r"(v) :: "memory");
        return v;
    }

    inline void WriteCR4(std::uint64_t value)
    {
        asm volatile("mov %0, %%cr4" : : "r"(value) : "memory");
    }

    inline std::uint64_t ReadCR4()
    {
        std::uint64_t v;
        asm volatile("mov %%cr4, %0" : "=r"(v) :: "memory");
        return v;
    }
}
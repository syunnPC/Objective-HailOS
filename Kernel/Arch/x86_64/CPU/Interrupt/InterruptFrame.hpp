#pragma once

#include "BaseType.hpp"

namespace Kernel::Boot::x86_64
{
    struct InterruptFrame
    {
        std::uint64_t RIP;
        std::uint64_t CS;
        std::uint64_t RFLAGS;
        std::uint64_t RSP;
        std::uint64_t SS;
    };
}
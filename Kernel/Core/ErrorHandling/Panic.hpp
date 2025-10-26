#pragma once

#include "BaseType.hpp"
#include "Halt.hpp"

#define PANIC_EX(status, p1, p2, p3, p4) Kernel::PanicEx(status, p1, p2, p3, p4, __FILE__, __func__, __LINE__)
#define PANIC(status, p1) Kernel::PanicEx(status, p1, 0, 0, 0, __FILE__, __func__, __LINE__)

namespace Kernel
{
    void PanicEx(Status status, std::uint64_t param1, std::uint64_t param2, std::uint64_t pararm3, std::uint64_t param4, const char* fileName = nullptr, const char* func = nullptr, int line = 0);
}
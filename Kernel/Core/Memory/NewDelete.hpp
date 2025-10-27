#pragma once

#include "MemoryInfo.hpp"

namespace Kernel::Early
{
    class EarlyPageAllocator;
    void InitOperatorNew(EarlyPageAllocator& alloc) noexcept;
}
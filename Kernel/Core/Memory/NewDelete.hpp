#pragma once

#include "MemoryInfo.hpp"
#include "IPageAllocator.hpp"

namespace Kernel::Early
{
    void InitOperatorNew(IPageAllocator& alloc) noexcept;
}
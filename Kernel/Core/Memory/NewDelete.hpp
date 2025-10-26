#include "MemoryInfo.hpp"
#include "EarlyPageAllocator.hpp"

namespace Kernel::Early
{
    void InitOperatorNew(EarlyPageAllocator& alloc) noexcept;
}
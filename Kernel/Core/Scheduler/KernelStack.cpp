#include "KernelStack.hpp"
#include "EarlyPageAllocator.hpp"

namespace Kernel::Sched
{
    constexpr std::size_t DEFAULT_STACK_SIZE = 16 * 1024;

    bool KernelStack::Allocate(std::size_t bytes)
    {
        if (bytes == 0)
        {
            bytes = DEFAULT_STACK_SIZE;
        }

        void* mem = Early::GetEarlyPageAllocator()->AllocatePage(bytes);
        if (mem == nullptr)
        {
            return false;
        }

        m_Base = mem;
        m_Size = ((bytes % PAGE_SIZE != 0) ? (bytes / PAGE_SIZE) + 1 : (bytes / PAGE_SIZE)) * PAGE_SIZE;
        return true;
    }

    void KernelStack::Free()
    {
        Early::GetEarlyPageAllocator()->FreePage(m_Base, m_Size);
    }
}
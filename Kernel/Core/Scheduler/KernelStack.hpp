#pragma once

#include "BaseType.hpp"

namespace Kernel::Sched
{
    class KernelStack
    {
    private:
        void* m_Base;
        std::size_t m_Size;
    public:
        KernelStack() noexcept : m_Base(nullptr), m_Size(0) {}
        bool Allocate(std::size_t bytes);
        void Free();
        inline void* Base() const noexcept
        {
            return m_Base;
        }

        std::size_t Size() const noexcept
        {
            return m_Size;
        }

        std::uintptr_t Top() const noexcept
        {
            return reinterpret_cast<std::uintptr_t>(m_Base) + m_Size;
        }
    };
}
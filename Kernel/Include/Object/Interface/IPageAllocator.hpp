#pragma once

#include "IKernelObject.hpp"

namespace Kernel
{
    class IPageAllocator : public virtual IKernelObject
    {
    public:
        virtual void* AllocatePage() noexcept = 0;
        virtual void FreePage(void* ptr) noexcept = 0;
        virtual std::size_t GetAvailableSize() const noexcept = 0;
    protected:
        virtual ~IPageAllocator() override = default;
    };
}
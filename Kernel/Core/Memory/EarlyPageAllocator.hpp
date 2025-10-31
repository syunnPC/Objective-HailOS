#pragma once

#include "BaseType.hpp"
#include "RefCountedBase.hpp"
#include "IPageAllocator.hpp"
#include "RefPtr.hpp"
#include "PhysicalMemoryBitmap.hpp"

#include <new>

namespace Kernel::Early
{
    class EarlyPageAllocator final : public RefCountedBase, public virtual IPageAllocator
    {
    public:
        EarlyPageAllocator() noexcept = default;
        EarlyPageAllocator(const EarlyPageAllocator&) noexcept = delete;
        EarlyPageAllocator(EarlyPageAllocator&&) noexcept = delete;
        virtual void* AllocatePage() noexcept override;

        /// @brief 最小サイズを満たす数のページを確保（例：1ページ4096、minSize = 5000なら2ページ割り当て）
        /// @param minSize 最小サイズ
        /// @return 確保した領域へのポインタ
        virtual void* AllocatePage(std::size_t minSize) noexcept;

        virtual void FreePage(void* ptr)noexcept override;
        virtual void FreePage(void* ptr, std::size_t allocatedSize)noexcept;
        virtual std::size_t GetAvailableSize() const noexcept override;
    protected:
        virtual ~EarlyPageAllocator() override = default;
    };

    void InitEarlyPageAllocator(MemoryInfo* info) noexcept;

    IPageAllocator* GetEarlyPageAllocator() noexcept;
}
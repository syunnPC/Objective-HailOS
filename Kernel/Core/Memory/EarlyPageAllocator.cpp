#include "EarlyPageAllocator.hpp"
#include "PhysicalMemoryBitmap.hpp"
#include "Bit.hpp"

namespace Kernel::Early
{
    static inline void* FindAvailableAddress(std::size_t pageCount) noexcept
    {
        const std::size_t total = MAX_PHYS_PAGE_BITMAP_INDEX * CHAR_BIT;
        if (total < pageCount)
        {
            return nullptr;
        }

        int run = 0;
        for (std::size_t i = 0; i < total; ++i)
        {
            const std::size_t idx = i / CHAR_BIT;
            const int b = static_cast<int>(i % CHAR_BIT);

            if (Bit(PhysicalMemoryPageBitmap[idx], b) == UNUSED_BIT)
            {
                ++run;
                if (run == pageCount)
                {
                    const std::size_t start = i - (pageCount - 1);
                    const std::size_t sidx = start / CHAR_BIT;
                    const int sbit = static_cast<int>(start % CHAR_BIT);
                    return reinterpret_cast<void*>(BITMAP_INDEX_BIT_TO_PAGE_ADDRESS(idx, b));
                }
            }
            else
            {
                run = 0;
            }
        }

        return nullptr;
    }

    void* EarlyPageAllocator::AllocatePage() noexcept
    {
        for (std::size_t i = 0; i < MAX_PHYS_PAGE_BITMAP_INDEX; ++i)
        {
            for (auto j = 0; j < CHAR_BIT; ++j)
            {
                if (Bit(PhysicalMemoryPageBitmap[i], j) == UNUSED_BIT)
                {
                    void* ptr = reinterpret_cast<void*>(BITMAP_INDEX_BIT_TO_PAGE_ADDRESS(i, j));
                    SetBit(PhysicalMemoryPageBitmap[i], j, USED_BIT);
                    return ptr;
                }
            }
        }

        return nullptr;
    }

    void* EarlyPageAllocator::AllocatePage(std::size_t minSize) noexcept
    {
        const std::size_t pageCount = (minSize % PAGE_SIZE != 0) ? (minSize / PAGE_SIZE) + 1 : (minSize / PAGE_SIZE);
        if (pageCount == 1)
        {
            return AllocatePage();
        }

        void* allocated = FindAvailableAddress(pageCount);
        if (allocated == nullptr)
        {
            return nullptr;
        }

        SetBitRange(PhysicalMemoryPageBitmap, MAX_PHYS_PAGE_BITMAP_INDEX, ADDRESS_TO_PAGE_BITMAP_ARRAY_INDEX(reinterpret_cast<std::uintptr_t>(allocated)), ADDRESS_TO_PAGE_BITMAP_BIT_OFFSET(reinterpret_cast<std::uintptr_t>(allocated)), pageCount, USED_BIT);
        return allocated;
    }

    void EarlyPageAllocator::FreePage(void* ptr) noexcept
    {
        auto i = ADDRESS_TO_PAGE_BITMAP_ARRAY_INDEX(reinterpret_cast<std::uintptr_t>(ptr));
        auto b = ADDRESS_TO_PAGE_BITMAP_BIT_OFFSET(reinterpret_cast<std::uintptr_t>(ptr));
        SetBit(PhysicalMemoryPageBitmap[i], b, UNUSED_BIT);
    }

    void EarlyPageAllocator::FreePage(void* ptr, std::size_t allocatedSize) noexcept
    {
        const std::size_t pageCount = (allocatedSize % PAGE_SIZE != 0) ? (allocatedSize / PAGE_SIZE) + 1 : (allocatedSize / PAGE_SIZE);
        auto i = ADDRESS_TO_PAGE_BITMAP_ARRAY_INDEX(reinterpret_cast<std::uintptr_t>(ptr));
        auto b = ADDRESS_TO_PAGE_BITMAP_BIT_OFFSET(reinterpret_cast<std::uintptr_t>(ptr));
        SetBitRange(PhysicalMemoryPageBitmap, MAX_PHYS_PAGE_BITMAP_INDEX, i, b, pageCount, UNUSED_BIT);
    }

    std::size_t EarlyPageAllocator::GetAvailableSize() const noexcept
    {
        std::size_t total = 0;

        for (std::size_t i = 0; i < MAX_PHYS_PAGE_BITMAP_INDEX; ++i)
        {
            for (auto j = 0; j < CHAR_BIT; ++j)
            {
                if (Bit(PhysicalMemoryPageBitmap[i], j) == UNUSED_BIT)
                {
                    total += PAGE_SIZE;
                }
            }
        }

        return total;
    }
}
#include "Paging.hpp"
#include "MemoryUtility.hpp"

namespace Kernel
{
    static constexpr std::uint64_t PG_P = 1ull << 0;
    static constexpr std::uint64_t PG_RW = 1ull << 1;
    static constexpr std::uint64_t PG_PS = 1ull << 7;
    static constexpr std::uint64_t PG_PWT = 1ull << 3;
    static constexpr std::uint64_t PG_PCD = 1ull << 4;
    static constexpr std::uint64_t PG_ADDR_MASK = 0x000FFFFFFFFFF000ull;

    static constexpr std::uint64_t PAGE_4K = 0x1000;
    static constexpr std::uint64_t PAGE_2M = 0x200000;
    static constexpr std::uint64_t PAGE_1G = 0x40000000;
    static constexpr std::uint64_t ENTIRIES = 512;

    static inline std::uintptr_t AlignUp(std::uintptr_t addr, std::uint64_t align)
    {
        return (addr + align - 1) & ~(align - 1);
    }

    static inline std::uintptr_t CalcMaxPhys(const MemoryInfo* info)
    {
        std::uintptr_t max = 0;
        for (std::size_t i = 0; i < info->FreeRegionCount; i++)
        {
            auto end = info->FreeMemory[i].Base + info->FreeMemory[i].Length;
            if (end > max)
            {
                max = end;
            }
        }
        for (std::size_t i = 0; i < info->ReservedRegionCount; i++)
        {
            auto end = info->ReservedMemory[i].Base + info->ReservedMemory[i].Length;
            if (end > max)
            {
                max = end;
            }
        }

        return max;
    }

    static inline bool IsMMIORegion(std::uintptr_t phys, const MemoryInfo* info)
    {
        for (std::size_t i = 0; i < info->ReservedRegionCount; i++)
        {
            std::uintptr_t base = info->ReservedMemory[i].Base;
            std::uintptr_t end = base + info->ReservedMemory[i].Length;
            if (phys >= base && phys < end)
            {
                return true;
            }
        }

        return false;
    }

    bool BuildIdentityMap2M(const MemoryInfo* info, std::uint64_t& outCR3Phys, IPageAllocator& allocator)
    {
        if (info == nullptr)
        {
            return false;
        }

        std::uintptr_t maxPhys = CalcMaxPhys(info);
        std::uintptr_t lim = AlignUp(maxPhys, PAGE_2M);
        std::uint64_t gibCount = AlignUp(maxPhys, PAGE_1G) / PAGE_1G;
        if (gibCount > ENTIRIES)
        {
            return false;
        }

        void* pml4 = allocator.AllocatePage();
        if (pml4 == nullptr)
        {
            return false;
        }

        void* pdpt = allocator.AllocatePage();
        if (pml4 == nullptr)
        {
            return false;
        }

        reinterpret_cast<std::uint64_t*>(pml4)[0] = (reinterpret_cast<std::uint64_t>(pdpt) & PG_ADDR_MASK) | PG_P | PG_RW;

        for (std::uint64_t g = 0; g < gibCount; g++)
        {
            void* pd = allocator.AllocatePage();
            if (pd == nullptr)
            {
                return false;
            }

            Library::Memory::FillMemory(pd, PAGE_4K, 0);

            reinterpret_cast<std::uint64_t*>(pdpt)[g] = (reinterpret_cast<std::uint64_t>(pd) & PG_ADDR_MASK) | PG_P | PG_RW;

            std::uint64_t base = g * PAGE_1G;

            for (std::uint64_t i = 0; i < ENTIRIES; i++)
            {
                std::uint64_t phys = base + i * PAGE_2M;
                if (phys >= lim)
                {
                    break;
                }

                std::uint64_t flags = PG_P | PG_RW | PG_PS;

                if (IsMMIORegion(phys, info))
                {
                    //キャッシュ無効化
                    flags |= PG_PCD | PG_PWT;
                }

                std::uint64_t entry = (phys & 0xFFFFFFFFFFE00000ull) | flags;
                reinterpret_cast<std::uint64_t*>(pd)[i] = entry;
            }
        }

        outCR3Phys = reinterpret_cast<std::uint64_t>(pml4);
        return true;
    }
}
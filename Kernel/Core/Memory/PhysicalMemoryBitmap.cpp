#include "PhysicalMemoryBitmap.hpp"
#include "MemoryUtility.hpp"
#include "Bit.hpp"

namespace Kernel
{
    std::uint8_t PhysicalMemoryPageBitmap[MAX_PHYS_PAGE_BITMAP_INDEX];

    static inline std::uintptr_t Align(std::uintptr_t addr, std::size_t align)
    {
        if (align <= 1)
        {
            return addr;
        }

        const std::uintptr_t a = static_cast<std::uintptr_t>(align);

        if ((a & (a - 1)) == 0)
        {
            return (addr + (a - 1)) & ~(a - 1);
        }

        const std::uintptr_t r = addr % a;
        return r ? (addr + (a - r)) : addr;
    }

    void ParseMemoryInfo(MemoryInfo* info) noexcept
    {
        static bool called = false;
        if (called)
        {
            return;
        }

        //すべてを使用中とマーク
        Library::Memory::FillMemory(PhysicalMemoryPageBitmap, MAX_PHYS_PAGE_BITMAP_INDEX, 0xFF);

        for (std::size_t i = 0; i < info->FreeRegionCount; ++i)
        {
            auto r = info->FreeMemory[i];

            //NULLをアロケーションしないように
            if (r.Base < PAGE_SIZE)
            {
                if (r.Base + r.Length < PAGE_SIZE)
                {
                    continue;
                }
                else
                {
                    //ここにページがあるなら何か別のことに使えるかも
                    r.Base = PAGE_SIZE;
                    r.Length = r.Base - PAGE_SIZE;
                }
            }

            if (r.Length < PAGE_SIZE)
            {
                //1ページ以下の空き領域は無視
                //ここも何かに使えると思う
                continue;
            }

            auto alignedBase = Align(r.Base, PAGE_SIZE);
            auto size = r.Base + r.Length - alignedBase;
            if (size < PAGE_SIZE)
            {
                //アラインしたら領域が足りない場合
                //ここも何かに使う
                continue;
            }

            auto idx = ADDRESS_TO_PAGE_BITMAP_ARRAY_INDEX(alignedBase);
            auto b = ADDRESS_TO_PAGE_BITMAP_BIT_OFFSET(alignedBase);
            const auto pageCount = size / PAGE_SIZE;
            if (pageCount == 0)
            {
                //ないはずだけど一応
                continue;
            }
            if (pageCount == 1)
            {
                SetBit(PhysicalMemoryPageBitmap[idx], b, UNUSED_BIT);
                continue;
            }
            else
            {
                SetBitRange(PhysicalMemoryPageBitmap, MAX_PHYS_PAGE_BITMAP_INDEX, idx, b, pageCount, UNUSED_BIT);
                continue;
            }
        }
    }
}
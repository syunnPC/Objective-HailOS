#pragma once

#include "BaseType.hpp"
#include "MemoryInfo.hpp"

/*
    TODO:
    これらの関数をconstexpr inlineにする
*/

//nページ目を示す
#define ADDRESS_TO_PAGE_INDEX(addr) (static_cast<std::uintptr_t>(addr) / Kernel::PAGE_SIZE)

//nページ目+x のxを求める
#define ADDRESS_TO_PAGE_OFFSET(addr) (static_cast<std::uintptr_t>(addr)  % Kernel::PAGE_SIZE)

//アドレスxを含むページはPhysicalMemoryPageBitmapのインデックス[n]のどこかのビット、そのnを求める
#define ADDRESS_TO_PAGE_BITMAP_ARRAY_INDEX(addr) ((static_cast<std::uintptr_t>(addr) / Kernel::PAGE_SIZE)/CHAR_BIT)

//アドレスxを含むページはPhysicalMemoryPageBitmapのインデックス[n]のどこかのビット、第何ビット（0~7）かを求める
#define ADDRESS_TO_PAGE_BITMAP_BIT_OFFSET(addr) ((static_cast<std::uintptr_t>(addr) / Kernel::PAGE_SIZE)%CHAR_BIT)

//ページとオフセットを使ってアドレスを求める
#define PAGE_OFFSET_TO_ADDRESS(page, offset) ((page * Kernel::PAGE_SIZE) + offset)

//インデックスとオフセットからアドレスを求める
#define BITMAP_INDEX_BIT_TO_PAGE_ADDRESS(index, bit) ((index * CHAR_BIT * Kernel::PAGE_SIZE) + (bit * Kernel::PAGE_SIZE))

namespace Kernel
{
    constexpr std::size_t MAX_MEMORY_SIZE_BYTES = 68719476736;
    constexpr std::size_t MAX_PHYS_PAGE_BITMAP_INDEX = ((MAX_MEMORY_SIZE_BYTES / PAGE_SIZE) / CHAR_BIT);
    constexpr bool UNUSED_BIT = false;
    constexpr bool USED_BIT = !UNUSED_BIT;
    extern std::uint8_t PhysicalMemoryPageBitmap[MAX_PHYS_PAGE_BITMAP_INDEX];

    /// @brief ブートローダーから渡されたMemoryInfoをビットマップにパース、起動直後に一度だけ呼ぶこと
    /// @param info ブートローダーから渡されたMemoryInfo
    void ParseMemoryInfo(MemoryInfo* info) noexcept;
}
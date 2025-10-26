#pragma once

#include "BaseType.hpp"

namespace Kernel
{
    constexpr auto PAGE_SIZE = 4096;
    constexpr auto MAX_REGIONS = 64;

    struct MemoryRegion
    {
        std::uintptr_t Base;
        std::size_t Length;
    } __attribute__((packed));

    struct MemoryInfo
    {
        MemoryRegion FreeMemory[MAX_REGIONS];
        std::size_t FreeRegionCount;
        MemoryRegion ReservedMemory[MAX_REGIONS];
        std::size_t ReservedRegionCount;
    } __attribute__((packed));
}
#pragma once

#include "BaseType.hpp"

namespace Kernel::Boot::x86_64
{
    struct TSS
    {
        std::uint32_t Reserved0;
        std::uint64_t Rsp0;
        std::uint64_t Rsp1;
        std::uint64_t Rsp2;
        std::uint64_t Reserved1;
        std::uint64_t Ist1;
        std::uint64_t Ist2;
        std::uint64_t Ist3;
        std::uint64_t Ist4;
        std::uint64_t Ist5;
        std::uint64_t Ist6;
        std::uint64_t Ist7;
        std::uint64_t Reserved2;
        std::uint16_t Reserved3;
        std::uint16_t IoMapBase;
    } __attribute__((packed));

    struct TSSDescriptor
    {
        std::uint16_t LimitLow;
        std::uint16_t BaseLow;
        std::uint8_t BaseMid;
        std::uint8_t Access;
        std::uint8_t Granularity;
        std::uint8_t BaseHigh;
        std::uint32_t BaseUpper;
        std::uint32_t Reserved;
    } __attribute__((packed));
}
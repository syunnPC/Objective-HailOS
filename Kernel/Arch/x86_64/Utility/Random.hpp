#pragma once

#include "BaseType.hpp"
#include "TSC.hpp"

namespace Kernel::Arch::x86_64
{
    static inline bool RdRand16(std::uint16_t* out)
    {
        std::uint8_t ok;
        std::uint16_t val;

        asm volatile(
            "rdrand %0; setc %1"
            : "=r"(val), "=qm"(ok)
            :
            : "cc"
            );

        if (ok)
        {
            *out = val;
        }

        return ok;
    }

    static inline bool RdRand32(std::uint32_t* out)
    {
        std::uint8_t ok;
        std::uint32_t val;

        asm volatile(
            "rdrand %0; setc %1"
            : "=r"(val), "=qm"(ok)
            :
            : "cc"
            );

        if (ok)
        {
            *out = val;
        }

        return ok;
    }

    static inline bool RdRand64(std::uint64_t* out)
    {
        std::uint8_t ok;
        std::uint64_t val;

        asm volatile(
            "rdrand %0; setc %1"
            : "=r"(val), "=qm"(ok)
            :
            : "cc"
            );

        if (ok)
        {
            *out = val;
        }

        return ok;
    }

    static constexpr auto RDRAND_TRY_MAX = 10;

    inline std::uint16_t HwGenerateRandom16()
    {
        std::uint16_t result;
        for (auto i = 0; i < RDRAND_TRY_MAX; ++i)
        {
            if (RdRand16(&result))
            {
                return result;
            }
        }

        return static_cast<std::uint16_t>(ReadTSC() ^ (ReadTSC() >> 16));
    }

    inline std::uint32_t HwGenerateRandom32()
    {
        std::uint32_t result;
        for (auto i = 0; i < RDRAND_TRY_MAX; ++i)
        {
            if (RdRand32(&result))
            {
                return result;
            }
        }

        return static_cast<std::uint32_t>(ReadTSC() ^ (ReadTSC() >> 20));
    }

    inline std::uint64_t HwGenerateRandom64()
    {
        std::uint64_t result;
        for (auto i = 0; i < RDRAND_TRY_MAX; ++i)
        {
            if (RdRand64(&result))
            {
                return result;
            }
        }

        return ReadTSC() ^ (ReadTSC() >> 24);
    }
}
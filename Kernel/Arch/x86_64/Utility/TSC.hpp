#pragma once

#include "BaseType.hpp"
#include "Timer.hpp"
#include "RefCountedBase.hpp"
#include "CPUID.hpp"

namespace Kernel::Arch::x86_64
{
    inline std::uint64_t ReadTSC()
    {
        std::uint32_t lo;
        std::uint32_t hi;
        asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
        return ((static_cast<std::uint64_t>(hi) << 32) | lo);
    }

    class TSC final : public virtual ITimer, public RefCountedBase
    {
    private:
        bool m_Invaliant;
        std::uint64_t m_Freq;

        inline static bool InternalIsInvaliantTSC() noexcept
        {
            std::uint32_t a, b, c, d;
            CPUID(0x80000007, 0, a, b, c, d);
            return (d & (1u << 8)) != 0;
        }

        /// @brief TSC周波数を取得
        /// @return 失敗したら0
        inline static std::uint64_t TryGetTSCFreq() noexcept
        {
            std::uint64_t result = 0;
            std::uint32_t a, b, c, d;
            CPUID(0x15, 0, a, b, c, d);
            if (a != 0 && b != 0 && c != 0)
            {
                return ((static_cast<std::uint64_t>(c) * b) / a);
            }

            CPUID(0x16, 0, a, b, c, d);
            auto base = a & 0xFFFFu;
            if (base != 0)
            {
                return ((static_cast<std::uint64_t>(base) * 1000ull * 1000ull));
            }

            return 0;
        }

    public:
        TSC() noexcept : m_Invaliant(InternalIsInvaliantTSC()), m_Freq(TryGetTSCFreq()) {}
        explicit TSC(HardwareClockInfo* info) noexcept : m_Invaliant(InternalIsInvaliantTSC()), m_Freq(info->TSCFreq) {}

        [[nodiscard]] inline bool IsInvaliantTSC() const noexcept
        {
            return m_Invaliant;
        }

        [[nodiscard]] inline virtual std::uint64_t GetFrequency() const noexcept
        {
            return m_Freq;
        }

        [[nodiscard]] inline virtual std::uint64_t GetCurrentStamp() const noexcept
        {
            return ReadTSC();
        }

        /// @brief このスリープ関数は使わないで
        /// @param value Unused
        /// @param timeScale Unused
        [[deprecated("Do not use this method")]] inline virtual void Sleep([[maybe_unused]] std::uint64_t value, [[maybe_unused]] TimeScale timeScale) noexcept
        {
            return;
        }
    protected:
        virtual ~TSC() noexcept override = default;
    };
}
#include "IO.hpp"
#include "LAPIC.hpp"
#include "TSC.hpp"
#include "InterruptDispatch.hpp"

namespace
{
    static inline void WriteMSR(std::uint32_t msr, std::uint64_t value)
    {
        std::uint32_t lo = static_cast<std::uint32_t>(value & 0xFFFFFFFFull);
        std::uint32_t hi = static_cast<std::uint32_t>(value >> 32);
        asm volatile("wrmsr" : : "c"(msr), "a"(lo), "d"(hi) : );
    }

    static inline std::uint64_t ReadMSR(std::uint32_t msr)
    {
        std::uint32_t lo, hi;
        asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr) : );
        return ((static_cast<std::uint64_t>(hi) << 32) | lo);
    }

    constexpr std::uint32_t MSR_IA32_APIC_BASE = 0x1B;
    constexpr std::uint64_t APIC_BASE_BSP = 1ull << 8;
    constexpr std::uint64_t APIC_BASE_EXTD = 1ull << 10;
    constexpr std::uint64_t APIC_BASE_EN = 1ull << 11;
    constexpr std::uint64_t APIC_BASE_ADDR_MASK = 0xFFFFF000ull;
    constexpr std::uint32_t LAPIC_ID = 0x020;
    constexpr std::uint32_t LAPIC_VERSION = 0x030;
    constexpr std::uint32_t LAPIC_TPR = 0x080;
    constexpr std::uint32_t LAPIC_EOI = 0x0B0;
    constexpr std::uint32_t LAPIC_SVR = 0x0F0;
    constexpr std::uint32_t LAPIC_ESR = 0x280;
    constexpr std::uint32_t LAPIC_ICR_LOW = 0x300;
    constexpr std::uint32_t LAPIC_ICR_HIGH = 0x310;
    constexpr std::uint32_t LAPIC_LVT_TIMER = 0x320;
    constexpr std::uint32_t LAPIC_LVT_THERMAL = 0x330;
    constexpr std::uint32_t LAPIC_LVT_PERF = 0x340;
    constexpr std::uint32_t LAPIC_LVT_LINT0 = 0x350;
    constexpr std::uint32_t LAPIC_LVT_LINT1 = 0x360;
    constexpr std::uint32_t LAPIC_LVT_ERROR = 0x370;
    constexpr std::uint32_t LAPIC_TIMER_INIT = 0x380;
    constexpr std::uint32_t LAPIC_TIMER_CURR = 0x390;
    constexpr std::uint32_t LAPIC_TIMER_DIV = 0x3E0;
    constexpr std::uint32_t SVR_APIC_ENABLE = 1u << 8;
    constexpr std::uint32_t SVR_SUPPRESS_BCAST_EOI = 1u << 12;
    constexpr std::uint32_t LVT_MASKED = 1u << 16;
    constexpr std::uint32_t LVT_PERIODIC = 1u << 17;

    volatile std::uint32_t* gLAPICMMIO = nullptr;
    bool gLAPICReady = false;

    inline std::uint32_t LAPICMMIORead(std::uint32_t offset) noexcept
    {
        return *reinterpret_cast<volatile std::uint32_t*>((std::uintptr_t)gLAPICMMIO + offset);
    }

    inline void LAPICMMIOWrite(std::uint32_t offset, std::uint32_t value) noexcept
    {
        *reinterpret_cast<volatile std::uint32_t*>((std::uintptr_t)gLAPICMMIO + offset) = value;
        (void)LAPICMMIORead(LAPIC_ID);
    }

    inline void CPUID_1(std::uint32_t& eax, std::uint32_t& ebx, std::uint32_t& ecx, std::uint32_t& edx) noexcept
    {
        eax = 1;
        asm volatile(
            "cpuid"
            : "+a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : : "cc"
            );
    }
}

namespace Kernel::Arch::x86_64::APIC
{
    bool IsPresent() noexcept
    {
        std::uint32_t a, b, c, d;
        CPUID_1(a, b, c, d);
        return (d & (1u << 9)) != 0;
    }

    void Enable() noexcept
    {
        auto apicBase = ReadMSR(MSR_IA32_APIC_BASE);
        apicBase |= APIC_BASE_EN;
        apicBase &= ~APIC_BASE_EXTD;

        WriteMSR(MSR_IA32_APIC_BASE, apicBase);

        std::uintptr_t base = static_cast<std::uintptr_t>(apicBase & APIC_BASE_ADDR_MASK);
        gLAPICMMIO = reinterpret_cast<volatile std::uint32_t*>(base);
    }

    void InitializeEarly(std::uint8_t spuriousVector, std::uint8_t errorVector) noexcept
    {
        if (!gLAPICMMIO)
        {
            Enable();
        }

        if (spuriousVector < 0x20)
        {
            spuriousVector = 0xFF;
        }
        if (spuriousVector < 0x20)
        {
            errorVector = 0xF1;
        }

        LAPICMMIOWrite(LAPIC_TPR, 0);

        LAPICMMIOWrite(LAPIC_LVT_TIMER, LVT_MASKED | 0xF0);
        LAPICMMIOWrite(LAPIC_LVT_PERF, LVT_MASKED | 0xF2);
        LAPICMMIOWrite(LAPIC_LVT_THERMAL, LVT_MASKED | 0xF3);
        LAPICMMIOWrite(LAPIC_LVT_LINT0, LVT_MASKED | 0xF4);
        LAPICMMIOWrite(LAPIC_LVT_LINT1, LVT_MASKED | 0xF5);
        LAPICMMIOWrite(LAPIC_LVT_ERROR, errorVector);

        LAPICMMIOWrite(LAPIC_ESR, 0);
        (void)LAPICMMIORead(LAPIC_ESR);

        LAPICMMIOWrite(LAPIC_SVR, SVR_APIC_ENABLE | spuriousVector);

        LAPICMMIOWrite(LAPIC_EOI, 0);

        gLAPICReady = true;
    }

    void EndOfInterrupt() noexcept
    {
        if (gLAPICMMIO)
        {
            LAPICMMIOWrite(LAPIC_EOI, 0);
        }
    }

    std::uint32_t ReadLAPICID() noexcept
    {
        return gLAPICMMIO ? (LAPICMMIORead(LAPIC_ID) >> 24) : 0xFFFFFFFFu;
    }

    std::uint32_t ReadLAPICVersion() noexcept
    {
        return gLAPICMMIO ? (LAPICMMIORead(LAPIC_VERSION) & 0xFFu) : 0xFFFFFFFFu;
    }

    void DisableLegacyPIC() noexcept
    {
        IO::Out8(0x21, 0xFF);
        IO::Out8(0xA1, 0xFF);
    }

    void ConfigureTimer(std::uint8_t vector, std::uint8_t divideShift, std::uint32_t initialCount, bool periodic) noexcept
    {
        if (gLAPICMMIO)
        {
            Enable();
        }

        std::uint32_t div = (divideShift & 7);
        LAPICMMIOWrite(LAPIC_TIMER_DIV, div);

        std::uint32_t lvt = vector | (periodic ? LVT_PERIODIC : 0);
        LAPICMMIOWrite(LAPIC_LVT_TIMER, lvt | LVT_MASKED);

        LAPICMMIOWrite(LAPIC_TIMER_INIT, initialCount);
        LAPICMMIOWrite(LAPIC_LVT_TIMER, lvt);
    }

    void MaskTimer(bool masked) noexcept
    {
        if (gLAPICMMIO)
        {
            return;
        }

        std::uint32_t lvt = LAPICMMIORead(LAPIC_LVT_TIMER);
        if (masked)
        {
            lvt |= LVT_MASKED;
        }
        else
        {
            lvt &= ~LVT_MASKED;
        }

        LAPICMMIOWrite(LAPIC_LVT_TIMER, lvt);
    }

    std::uint32_t CalibrateTimerInitialCountTSC(std::uint64_t tscHz, std::uint8_t divideShift, std::uint32_t targetPeriodUs, std::uint32_t sampleMs) noexcept
    {
        constexpr std::uint32_t FALLBACK_INITIAL_COUNT = 62500u;

        if (!gLAPICMMIO || tscHz == 0 || targetPeriodUs == 0 || sampleMs == 0)
        {
            return FALLBACK_INITIAL_COUNT;
        }

        LAPICMMIOWrite(LAPIC_TIMER_DIV, (divideShift & 7));

        std::uint32_t lvt = LAPICMMIORead(LAPIC_LVT_TIMER);
        lvt &= ~(LVT_PERIODIC);
        lvt |= LVT_MASKED;
        lvt = (lvt & 0xFFFFFF00u) | Interrupts::VEC_TIMER;
        LAPICMMIOWrite(LAPIC_LVT_TIMER, lvt);

        const std::uint32_t startCount = 0x7FFFFFFFu;
        LAPICMMIOWrite(LAPIC_TIMER_INIT, startCount);

        const std::uint64_t targetDelta = (tscHz * static_cast<std::uint64_t>(sampleMs)) / 1000ull;

        const std::uint64_t t0 = Arch::x86_64::ReadTSC();
        while (true)
        {
            const std::uint64_t t1 = Arch::x86_64::ReadTSC();
            if ((t1 - t0) >= targetDelta)
            {
                break;
            }
        }

        const std::uint32_t curr = LAPICMMIORead(LAPIC_TIMER_CURR);
        const std::uint64_t deltaApic = static_cast<std::uint64_t>(startCount - curr);
        if (deltaApic == 0)
        {
            return FALLBACK_INITIAL_COUNT;
        }

        const std::uint64_t countsPerSec = (deltaApic * 1000ull) / static_cast<std::uint64_t>(sampleMs);

        std::uint64_t initial64 = (countsPerSec * static_cast<std::uint64_t>(targetPeriodUs)) / 1000000ull;
        if (initial64 == 0)
        {
            initial64 = 1;
        }

        if (initial64 > 0xFFFFFFFFull)
        {
            initial64 = 0xFFFFFFFFull;
        }

        return static_cast<std::uint32_t>(initial64);
    }
}
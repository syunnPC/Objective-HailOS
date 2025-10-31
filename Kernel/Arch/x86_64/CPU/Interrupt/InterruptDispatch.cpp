#include "InterruptDispatch.hpp"
#include "PIC.hpp"
#include "MSR.hpp"

namespace Kernel::Arch::x86_64::Interrupts
{
    static InterruptHandler gHandlers[256]{};
    static volatile std::uint64_t gVectorCounts[256]{};

    static void (*gEOI)() noexcept = nullptr;
    static EventLogger gLogger = nullptr;

    static inline bool NeedsEOI(std::uint8_t vec) noexcept
    {
        if (vec == VEC_SPURIOUS)
        {
            return false;
        }

        if (vec == VEC_TIMER || vec == VEC_ERROR)
        {
            return true;
        }

        return (vec >= IRQ_BASE && vec <= 0xFE);
    }

    static void DefaultEOI() noexcept
    {
        constexpr std::uint32_t IA32_APIC_BASE_MSR = 0x1B;
        auto msr = MSR::Read(IA32_APIC_BASE_MSR);
        std::uintptr_t base = static_cast<std::uintptr_t>(msr & 0xFFFFF000ULL);
        volatile std::uint32_t* eoi = reinterpret_cast<volatile std::uint32_t*>(base + 0xB0);
        *eoi = 0u;
    }

    static inline void EmitEOI(std::uint8_t vec) noexcept
    {
        (void)vec;
        (gEOI ? gEOI : DefaultEOI) ();
    }

    void InitInterruptDispatch() noexcept
    {
        for (std::size_t i = 0; i < 256; ++i)
        {
            gHandlers[i] = nullptr;
            gVectorCounts[i] = 0;
        }

        gLogger = nullptr;
        gEOI = nullptr;
    }

    void SetEOIEmitter(void (*fn)() noexcept) noexcept
    {
        gEOI = fn;
    }

    void SetInterruptEventLogger(EventLogger fn) noexcept
    {
        gLogger = fn;
    }

    bool RegisterInterruptHandler(std::uint8_t vector, InterruptHandler fn) noexcept
    {
        if (fn == nullptr)
        {
            return false;
        }

        gHandlers[vector] = fn;
        return true;
    }

    bool UnregisterInterruptHandler(std::uint8_t vector, InterruptHandler fn) noexcept
    {
        if (gHandlers[vector] != fn)
        {
            return false;
        }

        gHandlers[vector] = nullptr;
        return true;
    }

    std::uint64_t GetVectorCount(std::uint8_t vector) noexcept
    {
        return gVectorCounts[vector];
    }

    void ResetVectorCounts() noexcept
    {
        for (auto& c : gVectorCounts)
        {
            c = 0;
        }
    }

    static inline void Dispatch(std::uint8_t vec, const InterruptFrame& frame, bool hasError, std::uint64_t error) noexcept
    {
        std::uint64_t count = ++gVectorCounts[vec];

        if (gLogger && count == 1)
        {
            InterruptEvent ev{ vec, hasError, error, &frame, count };
            gLogger(ev);
        }

        if (InterruptHandler fn = gHandlers[vec])
        {
            fn(frame, hasError, error);
        }

        if (NeedsEOI(vec))
        {
            if (vec == VEC_PIT_IRQ)
            {
                PIC::EOIMaster();
            }
            else if (PIC::IsSlaveVector(vec))
            {
                PIC::EOISlave();
            }
            else if (PIC::IsMasterVector(vec))
            {
                PIC::EOIMaster();
            }

            EmitEOI(vec);
        }
    }
}

using Kernel::Arch::x86_64::Interrupts::InterruptFrame;
static inline void DispatchNoError(std::uint8_t vector, const InterruptFrame* frame) noexcept
{
    Kernel::Arch::x86_64::Interrupts::Dispatch(vector, *frame, false, 0);
}

static inline void DispatchError(std::uint8_t vector, const InterruptFrame* frame, std::uint64_t errorCode) noexcept
{
    Kernel::Arch::x86_64::Interrupts::Dispatch(vector, *frame, true, errorCode);
}

#define DEF_NOERR(n) extern "C" __attribute__((interrupt)) void Handler##n(InterruptFrame* frame) noexcept {DispatchNoError(static_cast<std::uint8_t>(n), frame);}
#define DEF_ERR(n) extern "C" __attribute__((interrupt)) void Handler##n(InterruptFrame* frame, std::uint64_t ec) noexcept {DispatchError(static_cast<std::uint8_t>(n), frame, ec);}

DEF_NOERR(0)
DEF_NOERR(1)
DEF_NOERR(2)
DEF_NOERR(3)
DEF_NOERR(4)
DEF_NOERR(5)
DEF_NOERR(6)
DEF_NOERR(7)
DEF_ERR(8)
DEF_NOERR(9)
DEF_ERR(10)
DEF_ERR(11)
DEF_ERR(12)
DEF_ERR(13)
DEF_ERR(14)
DEF_NOERR(15)
DEF_NOERR(16)
DEF_ERR(17)
DEF_NOERR(18)
DEF_NOERR(19)
DEF_NOERR(20)
DEF_NOERR(21)
DEF_NOERR(22)
DEF_NOERR(23)
DEF_NOERR(24)
DEF_NOERR(25)
DEF_NOERR(26)
DEF_NOERR(27)
DEF_NOERR(28)
DEF_NOERR(29)
DEF_ERR(30)
DEF_NOERR(31)
DEF_NOERR(32)
DEF_NOERR(33)
DEF_NOERR(34)
DEF_NOERR(35)
DEF_NOERR(36)
DEF_NOERR(37)
DEF_NOERR(38)
DEF_NOERR(39)
DEF_NOERR(40)
DEF_NOERR(41)
DEF_NOERR(42)
DEF_NOERR(43)
DEF_NOERR(44)
DEF_NOERR(45)
DEF_NOERR(46)
DEF_NOERR(47)
DEF_NOERR(48)
DEF_NOERR(49)
DEF_NOERR(50)
DEF_NOERR(51)
DEF_NOERR(52)
DEF_NOERR(53)
DEF_NOERR(54)
DEF_NOERR(55)
DEF_NOERR(56)
DEF_NOERR(57)
DEF_NOERR(58)
DEF_NOERR(59)
DEF_NOERR(60)
DEF_NOERR(61)
DEF_NOERR(62)
DEF_NOERR(63)
DEF_NOERR(64)
DEF_NOERR(65)
DEF_NOERR(66)
DEF_NOERR(67)
DEF_NOERR(68)
DEF_NOERR(69)
DEF_NOERR(70)
DEF_NOERR(71)
DEF_NOERR(72)
DEF_NOERR(73)
DEF_NOERR(74)
DEF_NOERR(75)
DEF_NOERR(76)
DEF_NOERR(77)
DEF_NOERR(78)
DEF_NOERR(79)
DEF_NOERR(80)
DEF_NOERR(81)
DEF_NOERR(82)
DEF_NOERR(83)
DEF_NOERR(84)
DEF_NOERR(85)
DEF_NOERR(86)
DEF_NOERR(87)
DEF_NOERR(88)
DEF_NOERR(89)
DEF_NOERR(90)
DEF_NOERR(91)
DEF_NOERR(92)
DEF_NOERR(93)
DEF_NOERR(94)
DEF_NOERR(95)
DEF_NOERR(96)
DEF_NOERR(97)
DEF_NOERR(98)
DEF_NOERR(99)
DEF_NOERR(100)
DEF_NOERR(101)
DEF_NOERR(102)
DEF_NOERR(103)
DEF_NOERR(104)
DEF_NOERR(105)
DEF_NOERR(106)
DEF_NOERR(107)
DEF_NOERR(108)
DEF_NOERR(109)
DEF_NOERR(110)
DEF_NOERR(111)
DEF_NOERR(112)
DEF_NOERR(113)
DEF_NOERR(114)
DEF_NOERR(115)
DEF_NOERR(116)
DEF_NOERR(117)
DEF_NOERR(118)
DEF_NOERR(119)
DEF_NOERR(120)
DEF_NOERR(121)
DEF_NOERR(122)
DEF_NOERR(123)
DEF_NOERR(124)
DEF_NOERR(125)
DEF_NOERR(126)
DEF_NOERR(127)
DEF_NOERR(128)
DEF_NOERR(129)
DEF_NOERR(130)
DEF_NOERR(131)
DEF_NOERR(132)
DEF_NOERR(133)
DEF_NOERR(134)
DEF_NOERR(135)
DEF_NOERR(136)
DEF_NOERR(137)
DEF_NOERR(138)
DEF_NOERR(139)
DEF_NOERR(140)
DEF_NOERR(141)
DEF_NOERR(142)
DEF_NOERR(143)
DEF_NOERR(144)
DEF_NOERR(145)
DEF_NOERR(146)
DEF_NOERR(147)
DEF_NOERR(148)
DEF_NOERR(149)
DEF_NOERR(150)
DEF_NOERR(151)
DEF_NOERR(152)
DEF_NOERR(153)
DEF_NOERR(154)
DEF_NOERR(155)
DEF_NOERR(156)
DEF_NOERR(157)
DEF_NOERR(158)
DEF_NOERR(159)
DEF_NOERR(160)
DEF_NOERR(161)
DEF_NOERR(162)
DEF_NOERR(163)
DEF_NOERR(164)
DEF_NOERR(165)
DEF_NOERR(166)
DEF_NOERR(167)
DEF_NOERR(168)
DEF_NOERR(169)
DEF_NOERR(170)
DEF_NOERR(171)
DEF_NOERR(172)
DEF_NOERR(173)
DEF_NOERR(174)
DEF_NOERR(175)
DEF_NOERR(176)
DEF_NOERR(177)
DEF_NOERR(178)
DEF_NOERR(179)
DEF_NOERR(180)
DEF_NOERR(181)
DEF_NOERR(182)
DEF_NOERR(183)
DEF_NOERR(184)
DEF_NOERR(185)
DEF_NOERR(186)
DEF_NOERR(187)
DEF_NOERR(188)
DEF_NOERR(189)
DEF_NOERR(190)
DEF_NOERR(191)
DEF_NOERR(192)
DEF_NOERR(193)
DEF_NOERR(194)
DEF_NOERR(195)
DEF_NOERR(196)
DEF_NOERR(197)
DEF_NOERR(198)
DEF_NOERR(199)
DEF_NOERR(200)
DEF_NOERR(201)
DEF_NOERR(202)
DEF_NOERR(203)
DEF_NOERR(204)
DEF_NOERR(205)
DEF_NOERR(206)
DEF_NOERR(207)
DEF_NOERR(208)
DEF_NOERR(209)
DEF_NOERR(210)
DEF_NOERR(211)
DEF_NOERR(212)
DEF_NOERR(213)
DEF_NOERR(214)
DEF_NOERR(215)
DEF_NOERR(216)
DEF_NOERR(217)
DEF_NOERR(218)
DEF_NOERR(219)
DEF_NOERR(220)
DEF_NOERR(221)
DEF_NOERR(222)
DEF_NOERR(223)
DEF_NOERR(224)
DEF_NOERR(225)
DEF_NOERR(226)
DEF_NOERR(227)
DEF_NOERR(228)
DEF_NOERR(229)
DEF_NOERR(230)
DEF_NOERR(231)
DEF_NOERR(232)
DEF_NOERR(233)
DEF_NOERR(234)
DEF_NOERR(235)
DEF_NOERR(236)
DEF_NOERR(237)
DEF_NOERR(238)
DEF_NOERR(239)
DEF_NOERR(240)
DEF_NOERR(241)
DEF_NOERR(242)
DEF_NOERR(243)
DEF_NOERR(244)
DEF_NOERR(245)
DEF_NOERR(246)
DEF_NOERR(247)
DEF_NOERR(248)
DEF_NOERR(249)
DEF_NOERR(250)
DEF_NOERR(251)
DEF_NOERR(252)
DEF_NOERR(253)
DEF_NOERR(254)
DEF_NOERR(255)
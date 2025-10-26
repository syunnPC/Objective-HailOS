#pragma once

#include "BaseType.hpp"
#include "InterruptFrame.hpp"

namespace Kernel::Arch::x86_64::Interrupts
{
    constexpr std::uint8_t IRQ_BASE = 0x20;
    constexpr std::uint8_t VEC_TIMER = 0x60;
    constexpr std::uint8_t VEC_ERROR = 0x61;
    constexpr std::uint8_t VEC_SPURIOUS = 0xFF;

    struct InterruptFrame
    {
        std::uint64_t RIP;
        std::uint64_t CS;
        std::uint64_t RFLAGS;
        std::uint64_t RSP;
        std::uint64_t SS;
    };

    struct InterruptEvent
    {
        std::uint8_t Vector;
        bool HasErrorCode;
        std::uint64_t ErrorCode;
        const InterruptFrame* Frame;
        std::uint64_t Count;
    };

    using InterruptHandler = void(*)(const InterruptFrame& frame, bool hasErrorCode, std::uint64_t errorCode) noexcept;
    using EventLogger = void(*)(InterruptEvent& ev) noexcept;

    bool RegisterInterruptHandler(std::uint8_t vector, InterruptHandler fn) noexcept;
    bool UnregisterInterruptHandler(std::uint8_t vector, InterruptHandler fn) noexcept;

    void SetEOIEmitter(void (*fn)() noexcept) noexcept;

    void SetInterruptEventLogger(EventLogger fn) noexcept;

    std::uint64_t GetVectorCount(std::uint8_t vector) noexcept;
    void ResetVectorCounts() noexcept;

    void InitializeInterruptDispatch() noexcept;
}

#define DECL_HANDLER(n) __attribute__((interrupt)) void Handler##n(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept

extern "C"
{
    __attribute__((interrupt)) void Handler0(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler1(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler2(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler3(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler4(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler5(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler6(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler7(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler9(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler15(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler16(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler18(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler19(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler20(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler21(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler22(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler23(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler24(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler25(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler26(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler27(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler28(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler29(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;
    __attribute__((interrupt)) void Handler31(Kernel::Arch::x86_64::Interrupts::InterruptFrame*) noexcept;

    __attribute__((interrupt)) void Handler8(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;
    __attribute__((interrupt)) void Handler10(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;
    __attribute__((interrupt)) void Handler11(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;
    __attribute__((interrupt)) void Handler12(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;
    __attribute__((interrupt)) void Handler13(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;
    __attribute__((interrupt)) void Handler14(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;
    __attribute__((interrupt)) void Handler17(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;
    __attribute__((interrupt)) void Handler30(Kernel::Arch::x86_64::Interrupts::InterruptFrame*, std::uint64_t) noexcept;

    DECL_HANDLER(32);
    DECL_HANDLER(33);
    DECL_HANDLER(34);
    DECL_HANDLER(35);
    DECL_HANDLER(36);
    DECL_HANDLER(37);
    DECL_HANDLER(38);
    DECL_HANDLER(39);
    DECL_HANDLER(40);
    DECL_HANDLER(41);
    DECL_HANDLER(42);
    DECL_HANDLER(43);
    DECL_HANDLER(44);
    DECL_HANDLER(45);
    DECL_HANDLER(46);
    DECL_HANDLER(47);
    DECL_HANDLER(48);
    DECL_HANDLER(49);
    DECL_HANDLER(50);
    DECL_HANDLER(51);
    DECL_HANDLER(52);
    DECL_HANDLER(53);
    DECL_HANDLER(54);
    DECL_HANDLER(55);
    DECL_HANDLER(56);
    DECL_HANDLER(57);
    DECL_HANDLER(58);
    DECL_HANDLER(59);
    DECL_HANDLER(60);
    DECL_HANDLER(61);
    DECL_HANDLER(62);
    DECL_HANDLER(63);
    DECL_HANDLER(64);
    DECL_HANDLER(65);
    DECL_HANDLER(66);
    DECL_HANDLER(67);
    DECL_HANDLER(68);
    DECL_HANDLER(69);
    DECL_HANDLER(70);
    DECL_HANDLER(71);
    DECL_HANDLER(72);
    DECL_HANDLER(73);
    DECL_HANDLER(74);
    DECL_HANDLER(75);
    DECL_HANDLER(76);
    DECL_HANDLER(77);
    DECL_HANDLER(78);
    DECL_HANDLER(79);
    DECL_HANDLER(80);
    DECL_HANDLER(81);
    DECL_HANDLER(82);
    DECL_HANDLER(83);
    DECL_HANDLER(84);
    DECL_HANDLER(85);
    DECL_HANDLER(86);
    DECL_HANDLER(87);
    DECL_HANDLER(88);
    DECL_HANDLER(89);
    DECL_HANDLER(90);
    DECL_HANDLER(91);
    DECL_HANDLER(92);
    DECL_HANDLER(93);
    DECL_HANDLER(94);
    DECL_HANDLER(95);
    DECL_HANDLER(96);
    DECL_HANDLER(97);
    DECL_HANDLER(98);
    DECL_HANDLER(99);
    DECL_HANDLER(100);
    DECL_HANDLER(101);
    DECL_HANDLER(102);
    DECL_HANDLER(103);
    DECL_HANDLER(104);
    DECL_HANDLER(105);
    DECL_HANDLER(106);
    DECL_HANDLER(107);
    DECL_HANDLER(108);
    DECL_HANDLER(109);
    DECL_HANDLER(110);
    DECL_HANDLER(111);
    DECL_HANDLER(112);
    DECL_HANDLER(113);
    DECL_HANDLER(114);
    DECL_HANDLER(115);
    DECL_HANDLER(116);
    DECL_HANDLER(117);
    DECL_HANDLER(118);
    DECL_HANDLER(119);
    DECL_HANDLER(120);
    DECL_HANDLER(121);
    DECL_HANDLER(122);
    DECL_HANDLER(123);
    DECL_HANDLER(124);
    DECL_HANDLER(125);
    DECL_HANDLER(126);
    DECL_HANDLER(127);
    DECL_HANDLER(128);
    DECL_HANDLER(129);
    DECL_HANDLER(130);
    DECL_HANDLER(131);
    DECL_HANDLER(132);
    DECL_HANDLER(133);
    DECL_HANDLER(134);
    DECL_HANDLER(135);
    DECL_HANDLER(136);
    DECL_HANDLER(137);
    DECL_HANDLER(138);
    DECL_HANDLER(139);
    DECL_HANDLER(140);
    DECL_HANDLER(141);
    DECL_HANDLER(142);
    DECL_HANDLER(143);
    DECL_HANDLER(144);
    DECL_HANDLER(145);
    DECL_HANDLER(146);
    DECL_HANDLER(147);
    DECL_HANDLER(148);
    DECL_HANDLER(149);
    DECL_HANDLER(150);
    DECL_HANDLER(151);
    DECL_HANDLER(152);
    DECL_HANDLER(153);
    DECL_HANDLER(154);
    DECL_HANDLER(155);
    DECL_HANDLER(156);
    DECL_HANDLER(157);
    DECL_HANDLER(158);
    DECL_HANDLER(159);
    DECL_HANDLER(160);
    DECL_HANDLER(161);
    DECL_HANDLER(162);
    DECL_HANDLER(163);
    DECL_HANDLER(164);
    DECL_HANDLER(165);
    DECL_HANDLER(166);
    DECL_HANDLER(167);
    DECL_HANDLER(168);
    DECL_HANDLER(169);
    DECL_HANDLER(170);
    DECL_HANDLER(171);
    DECL_HANDLER(172);
    DECL_HANDLER(173);
    DECL_HANDLER(174);
    DECL_HANDLER(175);
    DECL_HANDLER(176);
    DECL_HANDLER(177);
    DECL_HANDLER(178);
    DECL_HANDLER(179);
    DECL_HANDLER(180);
    DECL_HANDLER(181);
    DECL_HANDLER(182);
    DECL_HANDLER(183);
    DECL_HANDLER(184);
    DECL_HANDLER(185);
    DECL_HANDLER(186);
    DECL_HANDLER(187);
    DECL_HANDLER(188);
    DECL_HANDLER(189);
    DECL_HANDLER(190);
    DECL_HANDLER(191);
    DECL_HANDLER(192);
    DECL_HANDLER(193);
    DECL_HANDLER(194);
    DECL_HANDLER(195);
    DECL_HANDLER(196);
    DECL_HANDLER(197);
    DECL_HANDLER(198);
    DECL_HANDLER(199);
    DECL_HANDLER(200);
    DECL_HANDLER(201);
    DECL_HANDLER(202);
    DECL_HANDLER(203);
    DECL_HANDLER(204);
    DECL_HANDLER(205);
    DECL_HANDLER(206);
    DECL_HANDLER(207);
    DECL_HANDLER(208);
    DECL_HANDLER(209);
    DECL_HANDLER(210);
    DECL_HANDLER(211);
    DECL_HANDLER(212);
    DECL_HANDLER(213);
    DECL_HANDLER(214);
    DECL_HANDLER(215);
    DECL_HANDLER(216);
    DECL_HANDLER(217);
    DECL_HANDLER(218);
    DECL_HANDLER(219);
    DECL_HANDLER(220);
    DECL_HANDLER(221);
    DECL_HANDLER(222);
    DECL_HANDLER(223);
    DECL_HANDLER(224);
    DECL_HANDLER(225);
    DECL_HANDLER(226);
    DECL_HANDLER(227);
    DECL_HANDLER(228);
    DECL_HANDLER(229);
    DECL_HANDLER(230);
    DECL_HANDLER(231);
    DECL_HANDLER(232);
    DECL_HANDLER(233);
    DECL_HANDLER(234);
    DECL_HANDLER(235);
    DECL_HANDLER(236);
    DECL_HANDLER(237);
    DECL_HANDLER(238);
    DECL_HANDLER(239);
    DECL_HANDLER(240);
    DECL_HANDLER(241);
    DECL_HANDLER(242);
    DECL_HANDLER(243);
    DECL_HANDLER(244);
    DECL_HANDLER(245);
    DECL_HANDLER(246);
    DECL_HANDLER(247);
    DECL_HANDLER(248);
    DECL_HANDLER(249);
    DECL_HANDLER(250);
    DECL_HANDLER(251);
    DECL_HANDLER(252);
    DECL_HANDLER(253);
    DECL_HANDLER(254);
    DECL_HANDLER(255);

}

#undef DECL_HANDLER
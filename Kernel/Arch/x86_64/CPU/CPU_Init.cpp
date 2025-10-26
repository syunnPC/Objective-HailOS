#include "CPU_Init.hpp"
#include "TSS.hpp"
#include "MemoryUtility.hpp"
#include "InterruptDispatch.hpp"

namespace Kernel::Boot::x86_64
{
    alignas(4096) std::uint8_t gGDT[7 * sizeof(GDTEntry)];
    alignas(4096) GDTR gGDTPtr;
    alignas(4096) TSS gTSS;
    alignas(4096) IDTEntry gIDT[IDT_ENTRIES];
    alignas(4096) IDTR gIDTPtr;

    namespace
    {
        inline void SetGDTEntry(GDTEntry* entry, std::uint32_t base, std::uint32_t limit, std::uint8_t access, std::uint8_t gran)
        {
            entry->LimitLow = static_cast<std::uint16_t>(limit & 0xFFFF);
            entry->BaseLow = static_cast<std::uint16_t>(base & 0xFFFF);
            entry->BaseMid = static_cast<std::uint8_t>((base >> 16) & 0xFF);
            entry->Access = access;
            entry->Granularity = static_cast<std::uint8_t>(((limit >> 16) & 0x0F) | (gran & 0xF0));
            entry->BaseHigh = static_cast<std::uint8_t>((base >> 24) & 0xFF);
        }

        inline void SetTSSDescriptor(TSSDescriptor* desc, std::uint64_t base, std::uint32_t limit, std::uint8_t access, std::uint8_t gran)
        {
            desc->LimitLow = static_cast<std::uint16_t>(limit & 0xFFFF);
            desc->BaseLow = static_cast<std::uint16_t>(base & 0xFFFF);
            desc->BaseMid = static_cast<std::uint8_t>((base >> 16) & 0xFF);
            desc->Access = access;
            desc->Granularity = static_cast<std::uint8_t>(((limit >> 16) & 0x0F) | (gran & 0xF0));
            desc->BaseHigh = static_cast<std::uint8_t>((base >> 24) & 0xFF);
            desc->BaseUpper = static_cast<std::uint32_t>((base >> 32) & 0xFFFFFFFF);
            desc->Reserved = 0;
        }

        inline void LoadGDT(const GDTR* gdtr)
        {
            asm volatile("lgdt (%0)" : : "r"(gdtr) : "memory");
        }
    }

    void ReloadSegmentsAndLoadTSS(std::uint16_t kernelDS, std::uint16_t kernelCS, std::uint16_t userDSRPL3, std::uint16_t userCSRPL3, std::uint16_t tssSel)
    {
        asm volatile(
            "mov %0, %%ax\n\t"
            "mov %%ax, %%ds\n\t"
            "mov %%ax, %%es\n\t"
            "mov %%ax, %%ss\n\t"
            "mov %%ax, %%fs\n\t"
            "mov %%ax, %%gs\n\t"
            : : "r"(kernelDS) : "rax"
            );

        asm volatile(
            "pushq %0\n\t"
            "lea 1f(%%rip), %%rax\n\t"
            "pushq %%rax\n\t"
            "lretq\n\t"
            "1:\n\t"
            : : "r"(static_cast<std::uint64_t>(kernelCS)) : "rax"
            );

        asm volatile("ltr %0" : : "r"(tssSel) : "memory");
    }

    void InitGDTAndTSS(void* kernelStackTop)
    {
        Library::Memory::FillMemory(gGDT, sizeof(gGDT), 0);
        Library::Memory::FillMemory(&gTSS, sizeof(gTSS), 0);

        GDTEntry* gdt = reinterpret_cast<GDTEntry*>(gGDT);

        SetGDTEntry(&gdt[1], 0, 0, 0x9A, 0xA0);
        SetGDTEntry(&gdt[2], 0, 0, 0x92, 0x00);
        SetGDTEntry(&gdt[3], 0, 0, 0xFA, 0xA0);
        SetGDTEntry(&gdt[4], 0, 0, 0xF2, 0x00);

        TSSDescriptor* tss = reinterpret_cast<TSSDescriptor*>(&gGDT[5 * sizeof(GDTEntry)]);
        std::uintptr_t tssBase = reinterpret_cast<std::uintptr_t>(&gTSS);
        std::uint32_t tssLimit = static_cast<std::uint32_t>(sizeof(TSS) - 1);

        SetTSSDescriptor(tss, tssBase, tssLimit, 0x89, 0x00);

        gTSS.Rsp0 = reinterpret_cast<std::uint64_t>(kernelStackTop);
        gTSS.IoMapBase = sizeof(TSS);

        gGDTPtr.Limit = static_cast<std::uint16_t>(sizeof(gGDT) - 1);
        gGDTPtr.Base = reinterpret_cast<std::uintptr_t>(gGDT);

        LoadGDT(&gGDTPtr);

        ReloadSegmentsAndLoadTSS(GDT_KERNEL_DS, GDT_KERNEL_CS, (GDT_USER_DS | 0x3), (GDT_USER_CS | 0x3), GDT_TSS_SEL);
    }

    void SetIDTEntry(int vec, void* handler, std::uint8_t typeAttribute)
    {
        std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(handler);
        gIDT[vec].OffsetLow = static_cast<std::uint16_t>(addr & 0xFFFF);
        gIDT[vec].Selector = GDT_KERNEL_CS;
        gIDT[vec].Ist = 0;
        gIDT[vec].TypeAttribute = typeAttribute;
        gIDT[vec].OffsetMid = static_cast<std::uint16_t>((addr >> 16) & 0xFFFF);
        gIDT[vec].OffsetHigh = static_cast<std::uint32_t>((addr >> 32));
        gIDT[vec].Reserved = 0;
    }

#define SET_VECTOR(n) SetIDTEntry(n, reinterpret_cast<void*>(::Handler##n), IDT_TYPE_INTERRUPT_GATE)

    void InitIDT(void)
    {
        SET_VECTOR(0);
        SET_VECTOR(1);
        SET_VECTOR(2);
        SET_VECTOR(3);
        SET_VECTOR(4);
        SET_VECTOR(5);
        SET_VECTOR(6);
        SET_VECTOR(7);
        SET_VECTOR(8);
        SET_VECTOR(9);
        SET_VECTOR(10);
        SET_VECTOR(11);
        SET_VECTOR(12);
        SET_VECTOR(13);
        SET_VECTOR(14);
        SET_VECTOR(15);
        SET_VECTOR(16);
        SET_VECTOR(17);
        SET_VECTOR(18);
        SET_VECTOR(19);
        SET_VECTOR(20);
        SET_VECTOR(21);
        SET_VECTOR(22);
        SET_VECTOR(23);
        SET_VECTOR(24);
        SET_VECTOR(25);
        SET_VECTOR(26);
        SET_VECTOR(27);
        SET_VECTOR(28);
        SET_VECTOR(29);
        SET_VECTOR(30);
        SET_VECTOR(31);
        SET_VECTOR(32);
        SET_VECTOR(33);
        SET_VECTOR(34);
        SET_VECTOR(35);
        SET_VECTOR(36);
        SET_VECTOR(37);
        SET_VECTOR(38);
        SET_VECTOR(39);
        SET_VECTOR(40);
        SET_VECTOR(41);
        SET_VECTOR(42);
        SET_VECTOR(43);
        SET_VECTOR(44);
        SET_VECTOR(45);
        SET_VECTOR(46);
        SET_VECTOR(47);
        SET_VECTOR(48);
        SET_VECTOR(49);
        SET_VECTOR(50);
        SET_VECTOR(51);
        SET_VECTOR(52);
        SET_VECTOR(53);
        SET_VECTOR(54);
        SET_VECTOR(55);
        SET_VECTOR(56);
        SET_VECTOR(57);
        SET_VECTOR(58);
        SET_VECTOR(59);
        SET_VECTOR(60);
        SET_VECTOR(61);
        SET_VECTOR(62);
        SET_VECTOR(63);
        SET_VECTOR(64);
        SET_VECTOR(65);
        SET_VECTOR(66);
        SET_VECTOR(67);
        SET_VECTOR(68);
        SET_VECTOR(69);
        SET_VECTOR(70);
        SET_VECTOR(71);
        SET_VECTOR(72);
        SET_VECTOR(73);
        SET_VECTOR(74);
        SET_VECTOR(75);
        SET_VECTOR(76);
        SET_VECTOR(77);
        SET_VECTOR(78);
        SET_VECTOR(79);
        SET_VECTOR(80);
        SET_VECTOR(81);
        SET_VECTOR(82);
        SET_VECTOR(83);
        SET_VECTOR(84);
        SET_VECTOR(85);
        SET_VECTOR(86);
        SET_VECTOR(87);
        SET_VECTOR(88);
        SET_VECTOR(89);
        SET_VECTOR(90);
        SET_VECTOR(91);
        SET_VECTOR(92);
        SET_VECTOR(93);
        SET_VECTOR(94);
        SET_VECTOR(95);
        SET_VECTOR(96);
        SET_VECTOR(97);
        SET_VECTOR(98);
        SET_VECTOR(99);
        SET_VECTOR(100);
        SET_VECTOR(101);
        SET_VECTOR(102);
        SET_VECTOR(103);
        SET_VECTOR(104);
        SET_VECTOR(105);
        SET_VECTOR(106);
        SET_VECTOR(107);
        SET_VECTOR(108);
        SET_VECTOR(109);
        SET_VECTOR(110);
        SET_VECTOR(111);
        SET_VECTOR(112);
        SET_VECTOR(113);
        SET_VECTOR(114);
        SET_VECTOR(115);
        SET_VECTOR(116);
        SET_VECTOR(117);
        SET_VECTOR(118);
        SET_VECTOR(119);
        SET_VECTOR(120);
        SET_VECTOR(121);
        SET_VECTOR(122);
        SET_VECTOR(123);
        SET_VECTOR(124);
        SET_VECTOR(125);
        SET_VECTOR(126);
        SET_VECTOR(127);
        SET_VECTOR(128);
        SET_VECTOR(129);
        SET_VECTOR(130);
        SET_VECTOR(131);
        SET_VECTOR(132);
        SET_VECTOR(133);
        SET_VECTOR(134);
        SET_VECTOR(135);
        SET_VECTOR(136);
        SET_VECTOR(137);
        SET_VECTOR(138);
        SET_VECTOR(139);
        SET_VECTOR(140);
        SET_VECTOR(141);
        SET_VECTOR(142);
        SET_VECTOR(143);
        SET_VECTOR(144);
        SET_VECTOR(145);
        SET_VECTOR(146);
        SET_VECTOR(147);
        SET_VECTOR(148);
        SET_VECTOR(149);
        SET_VECTOR(150);
        SET_VECTOR(151);
        SET_VECTOR(152);
        SET_VECTOR(153);
        SET_VECTOR(154);
        SET_VECTOR(155);
        SET_VECTOR(156);
        SET_VECTOR(157);
        SET_VECTOR(158);
        SET_VECTOR(159);
        SET_VECTOR(160);
        SET_VECTOR(161);
        SET_VECTOR(162);
        SET_VECTOR(163);
        SET_VECTOR(164);
        SET_VECTOR(165);
        SET_VECTOR(166);
        SET_VECTOR(167);
        SET_VECTOR(168);
        SET_VECTOR(169);
        SET_VECTOR(170);
        SET_VECTOR(171);
        SET_VECTOR(172);
        SET_VECTOR(173);
        SET_VECTOR(174);
        SET_VECTOR(175);
        SET_VECTOR(176);
        SET_VECTOR(177);
        SET_VECTOR(178);
        SET_VECTOR(179);
        SET_VECTOR(180);
        SET_VECTOR(181);
        SET_VECTOR(182);
        SET_VECTOR(183);
        SET_VECTOR(184);
        SET_VECTOR(185);
        SET_VECTOR(186);
        SET_VECTOR(187);
        SET_VECTOR(188);
        SET_VECTOR(189);
        SET_VECTOR(190);
        SET_VECTOR(191);
        SET_VECTOR(192);
        SET_VECTOR(193);
        SET_VECTOR(194);
        SET_VECTOR(195);
        SET_VECTOR(196);
        SET_VECTOR(197);
        SET_VECTOR(198);
        SET_VECTOR(199);
        SET_VECTOR(200);
        SET_VECTOR(201);
        SET_VECTOR(202);
        SET_VECTOR(203);
        SET_VECTOR(204);
        SET_VECTOR(205);
        SET_VECTOR(206);
        SET_VECTOR(207);
        SET_VECTOR(208);
        SET_VECTOR(209);
        SET_VECTOR(210);
        SET_VECTOR(211);
        SET_VECTOR(212);
        SET_VECTOR(213);
        SET_VECTOR(214);
        SET_VECTOR(215);
        SET_VECTOR(216);
        SET_VECTOR(217);
        SET_VECTOR(218);
        SET_VECTOR(219);
        SET_VECTOR(220);
        SET_VECTOR(221);
        SET_VECTOR(222);
        SET_VECTOR(223);
        SET_VECTOR(224);
        SET_VECTOR(225);
        SET_VECTOR(226);
        SET_VECTOR(227);
        SET_VECTOR(228);
        SET_VECTOR(229);
        SET_VECTOR(230);
        SET_VECTOR(231);
        SET_VECTOR(232);
        SET_VECTOR(233);
        SET_VECTOR(234);
        SET_VECTOR(235);
        SET_VECTOR(236);
        SET_VECTOR(237);
        SET_VECTOR(238);
        SET_VECTOR(239);
        SET_VECTOR(240);
        SET_VECTOR(241);
        SET_VECTOR(242);
        SET_VECTOR(243);
        SET_VECTOR(244);
        SET_VECTOR(245);
        SET_VECTOR(246);
        SET_VECTOR(247);
        SET_VECTOR(248);
        SET_VECTOR(249);
        SET_VECTOR(250);
        SET_VECTOR(251);
        SET_VECTOR(252);
        SET_VECTOR(253);
        SET_VECTOR(254);
        SET_VECTOR(255);

        gIDTPtr.Base = reinterpret_cast<std::uintptr_t>(&gIDT);
        gIDTPtr.Limit = sizeof(gIDT) - 1;

        asm volatile("lidt %0" : : "m"(gIDTPtr));
    }
}
#pragma once

#include "IDT.hpp"
#include "GDT.hpp"
#include "TSS.hpp"

namespace Kernel::Arch::x86_64
{
    extern std::uint8_t gGDT[7 * sizeof(GDTEntry)];
    extern GDTR gGDTPtr;
    extern TSS gTSS;
    extern IDTEntry gIDT[IDT_ENTRIES];
    extern IDTR gIDTPtr;

    extern "C"
    {
        extern std::uint8_t KernelStart[];
        extern std::uint8_t KernelEnd[];
        extern std::uint8_t KernelStackTop[];
    }

    void ReloadSegmentsAndLoadTSS(std::uint16_t kernelDS, std::uint16_t kernelCS, std::uint16_t userDSRPL3, std::uint16_t userCSRPL3, std::uint16_t tssSel);
    void InitGDTAndTSS(void* kernelStatckTop);
    void SetIDTEntry(int vec, void* handler, std::uint8_t typeAttribute);
    void InitIDT();
}
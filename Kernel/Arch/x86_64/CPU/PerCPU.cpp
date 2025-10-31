#include "PerCPU.hpp"
#include "MSR.hpp"
#include "EarlyPageAllocator.hpp"
#include "LAPIC.hpp"

namespace
{
    inline void WriteGS0(void* p) noexcept
    {
        asm volatile("mov %0, %%gs:0" :: "r"(p));
    }
}

namespace Kernel::Arch::x86_64
{
    void SetPerCPUBase(PerCPU* base) noexcept
    {
        const auto addr = reinterpret_cast<std::uintptr_t>(base);
        MSR::Write(MSR::IA32_GS_BASE, addr);
        MSR::Write(MSR::IA32_KERNEL_GS_BASE, addr);
    }

    void InitPerCPU() noexcept
    {
        auto allocator = Early::GetEarlyPageAllocator();
        void* mem = allocator->AllocatePage();
        auto pc = new(mem) PerCPU();
        pc->LAPICID = APIC::ReadLAPICID();
        pc->CPUIndex = 0;
        SetPerCPUBase(pc);
    }
}
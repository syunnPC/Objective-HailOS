#include "StringUtility.hpp"
#include "PortIO.hpp"
#include "CriticalSection.hpp"
#include "MemoryInfo.hpp"
#include "EarlyPageAllocator.hpp"
#include "NewDelete.hpp"
#include "PhysicalMemoryBitmap.hpp"
#include "Paging.hpp"
#include "KernelConsole.hpp"
#include "Panic.hpp"
#include "MADT.hpp"
#include "ACPIManager.hpp"
#include "APICController.hpp"
#include "IOAPIC.hpp"
#include "CPUInit.hpp"
#include "InterruptDispatch.hpp"
#include "ExceptionHandlers.hpp"
#include "Timer.hpp"
#include "TimerHandler.hpp"
#include "RedundantOutput.hpp"
#include "SerialConsole.hpp"
#include "PerCPU.hpp"
#include "Scheduler.hpp"
#include "CPUID.hpp"
#include "APICInit.hpp"
#include "ControlRegisters.hpp"

#include <new>

#define ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(type, bufferName) alignas(type) static std::uint8_t bufferName[sizeof(type)]
#define ALLOC_STATIC_BUFFER_ARRAY_FOR_PLACEMENT_NEW(type, count, bufferName) alignas(type) static std::uint8_t bufferName[sizeof(type) * count]

ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(Kernel::Arch::x86_64::APIC::APICController, apicControllerBuffer);
ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(Kernel::ACPI::ACPIManager, acpiManagerBuffer);

namespace
{
    struct BootInfo
    {
        char* Args;
        Kernel::MemoryInfo* MemInfo;
        Kernel::HardwareClockInfo* ClockInfo;
        Kernel::Early::GraphicInfo* FrameBufferInfo;
        Kernel::ACPI::RSDPtr* RSDP;
    } __attribute__((packed));

    void EnableFPUSSE() noexcept
    {
        auto cr0 = Kernel::Arch::x86_64::ReadCR0();
        cr0 &= ~(1ull << 2);
        cr0 |= (1ull << 1);
        cr0 |= (1ull << 5);
        Kernel::Arch::x86_64::WriteCR0(cr0);

        auto cr4 = Kernel::Arch::x86_64::ReadCR4();
        cr4 |= (1ull << 9);
        cr4 |= (1ull << 10);
        Kernel::Arch::x86_64::WriteCR4(cr4);
    }

    inline void InitCPU()
    {
        //GDT,TSS,IDTの初期化
        Kernel::Arch::x86_64::InitGDTAndTSS(Kernel::Arch::x86_64::KernelStackTop);
        Kernel::Arch::x86_64::InitIDT();
        EnableFPUSSE();
    }

    inline auto InitMemory(BootInfo* info)
    {
        std::uint64_t cr3;

        //メモリ関連の初期化
        ParseMemoryInfo(info->MemInfo);
        Kernel::Early::InitEarlyPageAllocator(info->MemInfo);
        auto allocator = Kernel::Early::GetEarlyPageAllocator();
        Kernel::Early::InitOperatorNew(*allocator);
        bool ok = Kernel::Arch::x86_64::BuildIdentityMap2M(info->MemInfo, cr3, *allocator);
        if (ok)
        {
            Kernel::Arch::x86_64::WriteCR3(cr3);
        }
        else
        {
            PANIC(Status::STATUS_ERROR, 0);
        }

        return allocator;
    }

    inline void InitPerCPUAndTime(BootInfo* info)
    {
        constexpr std::uint8_t DIV_SHIFT = 3;
        constexpr std::uint32_t TARGET_US = 1000;
        Kernel::Arch::x86_64::InitPerCPU();
        const auto initial = Kernel::Arch::x86_64::APIC::CalibrateTimerInitialCountTSC(info->ClockInfo->TSCFreq, DIV_SHIFT, TARGET_US, 200);
        Kernel::Arch::x86_64::Interrupts::InitTimerHandler(Kernel::Arch::x86_64::Interrupts::VEC_TIMER);
        Kernel::Arch::x86_64::APIC::ConfigureTimer(Kernel::Arch::x86_64::Interrupts::VEC_TIMER, DIV_SHIFT, initial, true);
    }

    void WorkerA(void*)
    {
        static volatile std::size_t ctr = 0;
        while (true)
        {
            auto con = Kernel::Early::GetBootstrapConsole();
            con->PutString("Hello, Scheduled tasks! This message must be shown once per a second!\n");
            Kernel::Sched::SleepMs(1000);
        }
    }

    void WorkerB(void*)
    {
        auto con = Kernel::Early::GetBootstrapConsole();
        volatile std::uint64_t next = Kernel::Arch::x86_64::CPU().TickCount + 4000;
        while (true)
        {
            if (Kernel::Arch::x86_64::CPU().TickCount >= next)
            {
                con->PutString("Greetings from Worker B!\n");
                next += 4000;
            }
        }
    }

    void WorkerC(void*)
    {
        volatile std::uint64_t ctr;
        while (true)
        {
            ++ctr;
        }
    }

    void KernelStartThreads()
    {
        Kernel::Sched::Init();
        Kernel::Sched::CreateAndEnqueue("WorkerA", &WorkerA, nullptr);
        Kernel::Sched::CreateAndEnqueue("WorkerB", &WorkerB, nullptr);
        Kernel::Sched::CreateAndEnqueue("WorkerC", &WorkerC, nullptr);
        Kernel::Sched::Start();
    }
}

extern "C" void main(BootInfo* info)
{
    using namespace Kernel;

    //割込み禁止
    Arch::x86_64::StartCriticalSection();
    //レガシPICをすべてマスク
    Arch::x86_64::APIC::DisableLegacyPIC();

    //CPU初期化（IDT, TSS, GDT）
    InitCPU();
    //メモリマネジメント機構を初期化
    auto allocator = InitMemory(info);

    //カーネルコンソールを初期化
    Early::InitKernelConsole(info->FrameBufferInfo);
    Early::InitSerialConsole();
    auto out = Early::GetBootstrapConsole();

    //LAPICの初期化
    auto apicController = new(apicControllerBuffer) Arch::x86_64::APIC::APICController();
    Arch::x86_64::Interrupts::InitInterruptDispatch();
    Arch::x86_64::Interrupts::InitCoreExceptionHandlers();

    //ACPI MADT解析
    ACPI::ParsedMADT madt{};
    bool ok = ACPI::ParseMADT(info->RSDP, madt);
    if (!ok)
    {
        out->PutString("Failed to parse MADT.\n");
    }
    else
    {
        //IOAPIC初期化
        Arch::x86_64::APIC::IOAPIC* ioapics = static_cast<Arch::x86_64::APIC::IOAPIC*>(allocator->AllocatePage(sizeof(Arch::x86_64::APIC::IOAPIC) * madt.IOAPICCount));
        for (std::size_t i = 0; i < madt.IOAPICCount; ++i)
        {
            (void)new(reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(ioapics) + i * sizeof(Arch::x86_64::APIC::IOAPIC))) Arch::x86_64::APIC::IOAPIC(madt.IOAPICs[i].Phys, madt.IOAPICs[i].GSIBase);
        }
    }

    InitPerCPUAndTime(info);

    out->PutString("PerCPU initialization finished. LAPIC ID=");
    out->PutString(Library::String::ULongToHexString(Arch::x86_64::CPU().LAPICID));
    out->PutChar('\n');

    SetHardwareTimerInfo(info->ClockInfo);
    auto acpiManager = new(acpiManagerBuffer) Kernel::ACPI::ACPIManager(info->RSDP);

    out->PutString("Finished kernel initialization.\n");

    {
        constexpr auto PROC_NAME_LEN = 48;
        static char procName[PROC_NAME_LEN + 1];
        auto regs = reinterpret_cast<std::uint32_t*>(procName);
        Arch::x86_64::CPUID(0x80000002, 0, regs[0], regs[1], regs[2], regs[3]);
        Arch::x86_64::CPUID(0x80000003, 0, regs[4], regs[5], regs[6], regs[7]);
        Arch::x86_64::CPUID(0x80000004, 0, regs[8], regs[9], regs[10], regs[11]);
        procName[PROC_NAME_LEN] = '\0';
        out->PutString("Processor: ");
        out->PutString(procName);
    }

    out->Flush();

    Arch::x86_64::EndCriticalSection();

    KernelStartThreads();

    while (true);
}
#include "StringUtility.hpp"
#include "IO.hpp"
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

#include "APICInit.hpp"

#include <new>

#define ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(type, bufferName) alignas(type) static std::uint8_t bufferName[sizeof(type)]
#define ALLOC_STATIC_BUFFER_ARRAY_FOR_PLACEMENT_NEW(type, count, bufferName) alignas(type) static std::uint8_t bufferName[sizeof(type) * count]

ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(Kernel::Early::EarlyPageAllocator, earlyPageAllocatorBuffer);
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

    inline void InitializeCPU()
    {
        //GDT,TSS,IDTの初期化
        Kernel::Arch::x86_64::InitGDTAndTSS(Kernel::Arch::x86_64::KernelStackTop);
        Kernel::Arch::x86_64::InitIDT();
    }

    inline auto InitializeMemory(BootInfo* info)
    {
        std::uint64_t cr3;

        //メモリ関連の初期化
        ParseMemoryInfo(info->MemInfo);
        Kernel::Early::EarlyPageAllocator* allocator = new(earlyPageAllocatorBuffer) Kernel::Early::EarlyPageAllocator();
        Kernel::Early::InitOperatorNew(*allocator);
        bool ok = Kernel::Arch::x86_64::BuildIdentityMap2M(info->MemInfo, cr3, *allocator);
        if (ok)
        {
            Kernel::Arch::x86_64::LoadCR3(cr3);
        }
        else
        {
            PANIC(Status::STATUS_ERROR, 0);
        }

        return allocator;
    }

    constexpr std::uint8_t DIV_SHIFT = 3;
    constexpr std::uint32_t TARGET_US = 10000;
}

extern "C" void main(BootInfo* info)
{
    using namespace Kernel;

    //割込み禁止
    Arch::x86_64::StartCriticalSection();
    //レガシPICをすべてマスク
    Arch::x86_64::APIC::DisableLegacyPIC();

    //CPU初期化（IDT, TSS, GDT）
    InitializeCPU();
    //メモリマネジメント機構を初期化
    auto allocator = InitializeMemory(info);

    //カーネルコンソールを初期化
    Early::InitKernelConsole(info->FrameBufferInfo);
    auto console = Early::GetKernelConsole();

    //LAPICの初期化
    auto apicController = new(apicControllerBuffer) Arch::x86_64::APIC::APICController();
    Arch::x86_64::Interrupts::InitializeInterruptDispatch();
    Arch::x86_64::Interrupts::InitCoreExceptionHandlers();
    Arch::x86_64::Interrupts::InitTimerHandler(Arch::x86_64::Interrupts::VEC_TIMER);

    //ACPI MADT解析
    ACPI::ParsedMADT madt{};
    bool ok = ACPI::ParseMADT(info->RSDP, madt);
    if (!ok)
    {
        PANIC(Status::STATUS_ERROR, 0);
    }

    //IOAPIC初期化
    Arch::x86_64::APIC::IOAPIC* ioapics = static_cast<Arch::x86_64::APIC::IOAPIC*>(allocator->AllocatePage(sizeof(Arch::x86_64::APIC::IOAPIC) * madt.IOAPICCount));
    for (std::size_t i = 0; i < madt.IOAPICCount; ++i)
    {
        (void)new(reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(ioapics) + i * sizeof(Arch::x86_64::APIC::IOAPIC))) Arch::x86_64::APIC::IOAPIC(madt.IOAPICs[i].Phys, madt.IOAPICs[i].GSIBase);
    }

    const auto initial = Arch::x86_64::APIC::CalibrateTimerInitialCountTSC(info->ClockInfo->TSCFreq, DIV_SHIFT, TARGET_US, 200);
    Arch::x86_64::APIC::ConfigureTimer(Arch::x86_64::Interrupts::VEC_TIMER, DIV_SHIFT, initial, true);

    Arch::x86_64::EndCriticalSection();

    SetHardwareTimerInfo(info->ClockInfo);
    auto acpiManager = new(acpiManagerBuffer) Kernel::ACPI::ACPIManager(info->RSDP);

    console->PutString("Finished kernel initialization.\n");

    while (true);
}
#include "KernelConsole.hpp"
#include "EarlyPageAllocator.hpp"
#include "ACPIManager.hpp"
#include "Timer.hpp"
#include "MemoryInfo.hpp"
#include "PhysicalMemoryBitmap.hpp"
#include "NewDelete.hpp"
#include "CriticalSection.hpp"
#include "APICInit.hpp"
#include "CPU_Init.hpp"
#include "Paging.hpp"
#include "APICController.hpp"
#include "InterruptDispatch.hpp"
#include "Panic.hpp"

#include <new>

#define ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(type, bufferName) alignas(type) static std::uint8_t bufferName[sizeof(type)]

namespace
{
    struct BootInfo
    {
        char* Args;
        Kernel::MemoryInfo* MemInfo;
        Kernel::HardwareClockInfo* ClockInfo;
        Kernel::Early::GraphicInfo* FrameBufferInfo;
        Kernel::RSDPtr* RSDP;
    } __attribute__((packed));
}

ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(Kernel::Early::EarlyPageAllocator, earlyPageAllocatorBuffer);
ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(Kernel::Arch::x86_64::APIC::APICController, apicControllerBuffer);
ALLOC_STATIC_BUFFER_FOR_PLACEMENT_NEW(Kernel::ACPIManager, acpiManagerBuffer);

extern "C" void main(BootInfo* info)
{
    using namespace Kernel;

    std::uint64_t cr3;

    Arch::x86_64::StartCriticalSection();

    Boot::x86_64::InitGDTAndTSS(Boot::x86_64::KernelStackTop);
    Boot::x86_64::InitIDT();

    Arch::x86_64::APIC::DisableLegacyPIC();

    ParseMemoryInfo(info->MemInfo);
    Kernel::Early::EarlyPageAllocator* allocator = new(earlyPageAllocatorBuffer) Kernel::Early::EarlyPageAllocator();
    Early::InitOperatorNew(*allocator);
    bool ok = BuildIdentityMap2M(info->MemInfo, cr3, *allocator);
    if (ok)
    {
        LoadCR3(cr3);
    }
    else
    {
        PANIC(Status::STATUS_ERROR, 0);
    }

    auto apicController = new(apicControllerBuffer) Arch::x86_64::APIC::APICController();
    Early::InitKernelConsole(info->FrameBufferInfo);
    auto console = Early::GetKernelConsole();

    SetHardwareTimerInfo(info->ClockInfo);
    auto acpiManager = new(acpiManagerBuffer) Kernel::ACPIManager(info->RSDP);

    Arch::x86_64::EndCriticalSection();

    console->PutString("Finished kernel initialization.\n");

    while (true);
}
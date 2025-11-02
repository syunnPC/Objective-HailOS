#include "PerCPU.hpp"
#include "Scheduler.hpp"
#include "Preempt.hpp"

extern "C" void SchedYieldTrampoline() noexcept
{
    Kernel::Sched::Yield();
}

extern "C" std::uint64_t GetPreemptResumeRIP() noexcept
{
    return Kernel::Sched::Current()->PreemptResumeRIP;
}

extern "C" void PreemptDisableForTrampoline() noexcept
{
    Kernel::Arch::x86_64::PreemptDisable();
}

extern "C" void PreemptEnableForTrampoline() noexcept
{
    Kernel::Arch::x86_64::PreemptEnable();
}

extern "C" void PreemptSetSavedRSP(std::uint64_t v) noexcept
{
    Kernel::Sched::Current()->TrampolineSavedRSP = v;
}

extern "C" std::uint64_t PreemptGetSavedRSP() noexcept
{
    return Kernel::Sched::Current()->TrampolineSavedRSP;
}
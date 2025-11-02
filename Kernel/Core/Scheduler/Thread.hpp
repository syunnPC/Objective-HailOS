#pragma once

#include "BaseType.hpp"
#include "KernelStack.hpp"
#include "PerCPU.hpp"
#include "RefCountedBase.hpp"
#include "RefPtr.hpp"
#include "FPU.hpp"

namespace Kernel::Sched
{
    enum class ThreadState : std::uint8_t
    {
        Init,
        Ready,
        Running,
        Sleep,
        Dead,
    };

    using ThreadEntry = void(*)(void*);

    struct ThreadContext
    {
        std::uint64_t RSP;
        alignas(64) std::uint8_t FXState[512];
    };

    class Thread final : public virtual IKernelObject, public RefCountedBase
    {
    public:
        std::uint64_t CanaryHead = 0xDEADBEEFCAFEBABEull;
        ThreadContext Ctx{};
        KernelStack KStack{};
        ThreadState State{ ThreadState::Init };
        ThreadEntry Entry{ nullptr };
        void* Arg{ nullptr };

        Thread* Next{ nullptr };
        Thread* Prev{ nullptr };
        const char* Name{ nullptr };
        std::uint32_t CPUIndex{ 0 };
        std::uint32_t TimeSliceTicks{ 5 };
        std::uint64_t TrampolineSavedRSP{ 0 };
        std::uint64_t PreemptResumeRIP{ 0 };
        std::uint64_t WakeTick{ 0 };
        bool InRunQueue{ false };
        std::uint32_t SavedPreemptDepth{ 0 };

        static RefPtr<Thread> CreateKernel(const char* name, ThreadEntry entry, void* arg, std::size_t stackSize = 0) noexcept;
        void PrepareInitialStack();
        std::uint64_t CanaryTail = 0x0123456789ABCDEFull;
    };

    inline Thread* Current() noexcept
    {
        return reinterpret_cast<Thread*>(Kernel::Arch::x86_64::CPU().CurrentThread);
    }

    extern "C" void ThreadStartTrampoline() noexcept;

    void Yield() noexcept;
    void SleepUntilTick(std::uint64_t targetTick) noexcept;
    void SleepMs(std::uint64_t ms) noexcept;
    std::uint64_t NowTick() noexcept;
}
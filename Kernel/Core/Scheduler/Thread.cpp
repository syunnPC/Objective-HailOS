#include "Thread.hpp"
#include "InterruptDispatch.hpp"
#include "PerCPU.hpp"
#include "Context.hpp"
#include "Scheduler.hpp"
#include "Halt.hpp"
#include "TSC.hpp"

namespace
{
    void ThreadExit() noexcept
    {
        auto t = Kernel::Sched::Current();
        t->State = Kernel::Sched::ThreadState::Dead;
        Kernel::Sched::Yield();
        Kernel::Arch::x86_64::HaltProcessor();
    }

    static inline std::uint64_t NowTick() noexcept
    {
        return Kernel::Arch::x86_64::CPU().TickCount;
    }
}

namespace Kernel::Sched
{
    void SleepUntilTick(std::uint64_t targetTick) noexcept
    {
        while (NowTick() < targetTick)
        {
            Kernel::Sched::Yield();
        }
    }

    void SleepMs(std::uint64_t ms) noexcept
    {
        const auto target = NowTick() + ms;
        SleepUntilTick(target);
    }

    RefPtr<Thread> Thread::CreateKernel(const char* name, ThreadEntry entry, void* arg, std::size_t stackSize) noexcept
    {
        auto t = RefPtr<Thread>(new Thread());
        t->Name = name;
        t->Entry = entry;
        t->Arg = arg;
        t->CPUIndex = Kernel::Arch::x86_64::CPU().CPUIndex;
        if (!t->KStack.Allocate(stackSize))
        {
            return nullptr;
        }
        t->PrepareInitialStack();
        t->State = ThreadState::Ready;
        return t;
    }

    void Thread::PrepareInitialStack()
    {
        std::uintptr_t sp = KStack.Top();
        auto push = [&](std::uint64_t v) { sp -= sizeof(v); *reinterpret_cast<std::uint64_t*>(sp) = v; };

        push(0);
        push(reinterpret_cast<std::uint64_t>(&ThreadStartTrampoline));
        push(0);
        push(0);
        push(0);
        push(0);
        push(0);
        push(0);

        Ctx.RSP = sp;
    }

    extern "C" void ThreadStartTrampoline() noexcept
    {
        Thread* self = Current();
        self->State = ThreadState::Running;
        self->Entry(self->Arg);
        ThreadExit();
    }
}
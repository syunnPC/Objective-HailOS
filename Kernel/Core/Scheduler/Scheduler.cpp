#include "Scheduler.hpp"
#include "PerCPU.hpp"
#include "Thread.hpp"
#include "APICController.hpp"
#include "InterruptDispatch.hpp"
#include "Context.hpp"

namespace
{
    static Kernel::Sched::RunQueue gRunQueue;
    static Kernel::Sched::Thread* gBootstapThread = nullptr;
    static Kernel::Sched::Thread* gIdleThread = nullptr;
    static volatile bool gNeedReschedule = false;

    static void IdleLoop() noexcept
    {
        while (true)
        {
            asm volatile("hlt");
        }
    }
}

namespace Kernel::Sched
{
    void IdleMain(void*) noexcept
    {
        IdleLoop();
    }

    void Init() noexcept
    {
        auto pc = &Kernel::Arch::x86_64::CPU();
        gBootstapThread = new Thread();
        gBootstapThread->Name = "Bootstrap";
        gBootstapThread->State = ThreadState::Running;
        gBootstapThread->CPUIndex = pc->CPUIndex;
        pc->CurrentThread = gBootstapThread;

        auto idle = Thread::CreateKernel("idle", &IdleMain, nullptr);
        gIdleThread = idle.Get();
    }

    void Start() noexcept
    {
        Yield();
    }

    void EnqueueReady(Thread* t) noexcept
    {
        t->State = ThreadState::Ready;
        gRunQueue.Push(t);
    }

    Thread* DequeueNext() noexcept
    {
        auto next = gRunQueue.PopFront();
        if (next == nullptr)
        {
            next = gIdleThread;
        }
        return next;
    }

    void SwitchTo(Thread* next) noexcept
    {
        auto pc = &Kernel::Arch::x86_64::CPU();
        auto curr = reinterpret_cast<Thread*>(pc->CurrentThread);
        if (next == curr)
        {
            return;
        }

        next->State = ThreadState::Running;
        pc->CurrentThread = next;
        Arch::x86_64::ArchSwitchContext(&curr->Ctx.RSP, next->Ctx.RSP);
    }

    void OnTimerTick() noexcept
    {
        gNeedReschedule = true;
    }

    void Yield() noexcept
    {
        auto pc = &Kernel::Arch::x86_64::CPU();
        auto curr = reinterpret_cast<Thread*>(pc->CurrentThread);

        if (curr != gIdleThread && curr != gBootstapThread && curr->State == ThreadState::Running)
        {
            EnqueueReady(curr);
        }

        auto next = DequeueNext();
        SwitchTo(next);
    }

    Thread* CreateAndEnqueue(const char* name, ThreadEntry entry, void* arg, std::size_t stackSize)
    {
        auto t = Thread::CreateKernel(name, entry, arg, stackSize);
        if (t)
        {
            EnqueueReady(t.Get());
        }

        return t.Get();
    }
}
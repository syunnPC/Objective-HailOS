#include "Scheduler.hpp"
#include "PerCPU.hpp"
#include "Thread.hpp"
#include "APICController.hpp"
#include "InterruptDispatch.hpp"
#include "Context.hpp"
#include "Preempt.hpp"
#include "CriticalSection.hpp"

#include "KernelConsole.hpp"

namespace
{
    static Kernel::Sched::RunQueue gRunQueue;
    static Kernel::Sched::RunQueue gSleepQueue;
    static Kernel::Sched::Thread* gBootstrapThread = nullptr;
    static Kernel::Sched::Thread* gIdleThread = nullptr;
    static volatile bool gNeedReschedule = false;
    static std::uint32_t gTimeSliceTicks = 5;
    static std::uint32_t gTicksSinceYield = 0;

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
    void SleepInsert(Thread* t, std::uint64_t wakeTick)
    {
        Arch::x86_64::PreemptGuard pg;

        t->WakeTick = wakeTick;

        if (gSleepQueue.Empty())
        {
            gSleepQueue.Push(t);
            return;
        }

        Thread* cur = gSleepQueue.Head;
        while (cur && cur->WakeTick <= wakeTick)
        {
            cur = cur->Next;
        }

        if (!cur)
        {
            gSleepQueue.Push(t);
            return;
        }

        if (cur == gSleepQueue.Head)
        {
            t->Prev = nullptr;
            t->Next = cur;
            cur->Prev = t;
            gSleepQueue.Head = t;
            return;
        }

        t->Next = cur;
        t->Prev = cur->Prev;
        cur->Prev->Next = t;
        cur->Prev = t;
    }

    static inline Thread* SleepPeekFront() noexcept
    {
        return gSleepQueue.Head;
    }

    static inline Thread* SleepPopFront() noexcept
    {
        Arch::x86_64::PreemptGuard pg;
        return gSleepQueue.PopFront();
    }

    void IdleMain(void*) noexcept
    {
        IdleLoop();
    }

    void Init() noexcept
    {
        auto pc = &Kernel::Arch::x86_64::CPU();
        gBootstrapThread = new Thread();
        gBootstrapThread->Name = "Bootstrap";
        gBootstrapThread->State = ThreadState::Running;
        gBootstrapThread->CPUIndex = pc->CPUIndex;
        Arch::x86_64::FPUInitState(gBootstrapThread->Ctx.FXState);
        pc->CurrentThread = gBootstrapThread;

        auto idle = Thread::CreateKernel("idle", &IdleMain, nullptr);
        gTicksSinceYield = 0;
        gIdleThread = idle.Get();
    }

    void Start() noexcept
    {
        Yield();
    }

    void EnqueueReady(Thread* t) noexcept
    {
        if (!t)
        {
            return;
        }

        if (t->CanaryHead != 0xDEADBEEFCAFEBABEull || t->CanaryTail != 0x0123456789ABCDEFull)
        {
            Kernel::Early::GetBootstrapConsole()->PutString("[CANARY TRIP] Thread memory corrupted!\n");
            asm volatile("xchg %bx, %bx");
        }

        t->State = ThreadState::Ready;

        if (!t->InRunQueue)
        {
            gRunQueue.Push(t);
            t->InRunQueue = true;
        }
    }

    Thread* DequeueNext() noexcept
    {
        auto next = gRunQueue.PopFront();
        if (next == nullptr)
        {
            next = gIdleThread;
        }
        else
        {
            next->InRunQueue = false;
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

        if (next->CanaryHead != 0xDEADBEEFCAFEBABEull || next->CanaryTail != 0x0123456789ABCDEFull || curr->CanaryHead != 0xDEADBEEFCAFEBABEull || curr->CanaryTail != 0x0123456789ABCDEFull)
        {
            Kernel::Early::GetBootstrapConsole()->PutString("[CANARY TRIP] Thread memory corrupted!\n");
            asm volatile("xchg %bx, %bx");
        }

        Arch::x86_64::FPUSave(curr->Ctx.FXState);
        Arch::x86_64::FPURestore(next->Ctx.FXState);

        next->State = ThreadState::Running;
        pc->CurrentThread = next;

        Arch::x86_64::ArchSwitchContext(&curr->Ctx.RSP, next->Ctx.RSP);
    }

    void OnTimerTick() noexcept
    {
        const auto now = NowTick();

        while (auto t = SleepPeekFront())
        {
            if (t->WakeTick > now)
            {
                break;
            }

            (void)SleepPopFront();
            t->State = ThreadState::Ready;
            EnqueueReady(t);
        }

        if (++gTicksSinceYield >= gTimeSliceTicks)
        {
            gNeedReschedule = true;
        }
    }

    bool NeedReschedule() noexcept
    {
        return gNeedReschedule;
    }

    void ClearReschedule() noexcept
    {
        gNeedReschedule = false;
    }

    void Yield() noexcept
    {
        auto flags = Arch::x86_64::SaveRFLAGS();
        Arch::x86_64::DisableInterrupt();
        Arch::x86_64::PreemptDisable();

        auto pc = &Kernel::Arch::x86_64::CPU();
        auto curr = reinterpret_cast<Thread*>(pc->CurrentThread);

        gTicksSinceYield = 0;
        gNeedReschedule = false;

        if (curr && curr->State == ThreadState::Running && curr != gIdleThread && curr != gBootstrapThread)
        {
            curr->State = ThreadState::Ready;
            EnqueueReady(curr);
        }

        Thread* next = DequeueNext();
        if (!next)
        {
            next = gIdleThread;
        }

        if (next == curr || next == nullptr)
        {
            if (curr)
            {
                curr->State = ThreadState::Running;
            }

            Arch::x86_64::PreemptEnable();
            Arch::x86_64::InterruptRestore(flags);
            return;
        }

        SwitchTo(next);

        Arch::x86_64::PreemptEnable();
        Arch::x86_64::InterruptRestore(flags);
    }

    void SetTimeSliceTicks(std::uint32_t ticks) noexcept
    {
        if (ticks == 0)
        {
            ticks = 1;
        }

        gTimeSliceTicks = ticks;
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
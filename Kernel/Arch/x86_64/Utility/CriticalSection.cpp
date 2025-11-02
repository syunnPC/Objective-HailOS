namespace Kernel::Arch::x86_64
{
    static bool InCritical{ false };

    void StartCriticalSection()
    {
        InCritical = true;
        asm volatile("cli" ::: "memory");
    }

    void EndCriticalSection()
    {
        InCritical = false;
        asm volatile("sti" ::: "memory");
    }

    void EnableInterrupt()
    {
        if (!InCritical)
        {
            asm volatile("sti" ::: "memory");
        }
    }

    void DisableInterrupt()
    {
        asm volatile("cli" ::: "memory");
    }
}
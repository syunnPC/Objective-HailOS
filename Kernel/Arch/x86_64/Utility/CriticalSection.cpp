namespace Kernel::Arch::x86_64
{
    static bool InCritical{ false };

    void StartCriticalSection()
    {
        InCritical = true;
        asm volatile("cli");
    }

    void EndCriticalSection()
    {
        InCritical = false;
        asm volatile("sti");
    }

    void EnableInterrupt()
    {
        if (!InCritical)
        {
            asm volatile("sti");
        }
    }

    void DisableInterrupt()
    {
        asm volatile("cli");
    }
}
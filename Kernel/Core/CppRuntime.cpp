extern "C"
{
    void __cxa_pure_virtual()
    {
        return;
    }

    struct __guard
    {
        unsigned char initialized;
    };

    int __cxa_guard_acquire(__guard* g)
    {
        return !g->initialized;
    }

    void __cxa_guard_release(__guard* g)
    {
        g->initialized = 1;
    }

    void __cxa_guard_abort(__guard*) {}

    int  __cxa_atexit(void (*)(void*), void*, void*)
    {
        return 0;
    }

    void __cxa_finalize(void*) {}

    void* __dso_handle = (void*)&__dso_handle;

    namespace std
    {
        [[gnu::noreturn]] void __glibcxx_assert_fail(const char* file, int line, const char* func, const char* expr) noexcept
        {
            asm volatile("cli; hlt");
            __builtin_unreachable();
        }
    }
}

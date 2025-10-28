#include "ExceptionHandlers.hpp"
#include "InterruptDispatch.hpp"
#include "StringUtility.hpp"
#include "Halt.hpp"
#include "KernelConsole.hpp"
#include "Paging.hpp"

namespace
{
    void DumpException(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& f, const char* name, bool hasError, std::uint64_t errorCode) noexcept
    {
        auto con = Kernel::Early::GetKernelConsole();
        con->Reset();
        con->PutString("===== x86-64 Processor Exception =====\n");
        con->PutString(name);
        con->PutString("\nRIP   :");
        con->PutString(Library::String::ULongToHexString(f.RIP));
        con->PutString("\nRSP   :");
        con->PutString(Library::String::ULongToHexString(f.RSP));
        con->PutString("\nCS    :");
        con->PutString(Library::String::ULongToHexString(f.CS));
        con->PutString("\nSS    :");
        con->PutString(Library::String::ULongToHexString(f.SS));
        con->PutString("\nRFLAGS:");
        con->PutString(Library::String::ULongToHexString(f.RFLAGS));
        if (hasError)
        {
            con->PutString("ERR   :");
            con->PutString(Library::String::ULongToHexString(errorCode));
        }

        auto cr2 = Kernel::Arch::x86_64::ReadCR3();
        if (cr2)
        {
            con->PutString("CR2   :");
            con->PutString(Library::String::ULongToHexString(cr2));
        }
    }

    void HandlerUD(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& frame, bool hasError, std::uint64_t errorCode) noexcept
    {
        DumpException(frame, "#UD", hasError, errorCode);
        Kernel::Arch::x86_64::HaltProcessor();
    }

    void HandlerGP(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& frame, bool hasError, std::uint64_t errorCode) noexcept
    {
        DumpException(frame, "#GP", hasError, errorCode);
        Kernel::Arch::x86_64::HaltProcessor();
    }

    void HandlerPF(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& frame, bool hasError, std::uint64_t errorCode) noexcept
    {
        DumpException(frame, "#PF", hasError, errorCode);
        Kernel::Arch::x86_64::HaltProcessor();
    }

    void HandlerDF(const Kernel::Arch::x86_64::Interrupts::InterruptFrame& frame, bool hasError, std::uint64_t errorCode) noexcept
    {
        DumpException(frame, "#DF", hasError, errorCode);
        Kernel::Arch::x86_64::HaltProcessor();
    }
}

namespace Kernel::Arch::x86_64::Interrupts
{
    void InitCoreExceptionHandlers() noexcept
    {
        RegisterInterruptHandler(6, &HandlerUD);
        RegisterInterruptHandler(8, &HandlerDF);
        RegisterInterruptHandler(13, &HandlerGP);
        RegisterInterruptHandler(14, &HandlerPF);
    }
}
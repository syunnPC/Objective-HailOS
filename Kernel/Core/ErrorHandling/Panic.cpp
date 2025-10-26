#include "KernelConsole.hpp"
#include "Halt.hpp"
#include "CriticalSection.hpp"
#include "StringUtility.hpp"

namespace Kernel
{
    void PanicEx(Status status, std::uint64_t param1, std::uint64_t param2, std::uint64_t param3, std::uint64_t param4, const char* fileName = nullptr, const char* func = nullptr, int line = -1)
    {
        Kernel::Arch::x86_64::StartCriticalSection();

        auto con = Early::GetKernelConsole();
        if (!con)
        {
            Arch::x86_64::HaltProcessor();
        }

        con->Reset();
        con->PutString("Panic! Status:");
        con->PutString(Library::String::ULongToHexString(static_cast<std::uint64_t>(status)));

        if (fileName != nullptr)
        {
            con->PutString("\nIn file:");
            con->PutString(fileName);
        }

        if (line >= 0)
        {
            con->PutString("\nLine:");
            con->PutString(fileName);
        }

        if (func != nullptr)
        {
            con->PutString("\nFunction:");
            con->PutString(func);
        }

        con->PutString("\nParam1:");
        con->PutString(Library::String::ULongToHexString(static_cast<std::uint64_t>(param1)));
        con->PutString("\nParam2:");
        con->PutString(Library::String::ULongToHexString(static_cast<std::uint64_t>(param2)));
        con->PutString("\nParam3:");
        con->PutString(Library::String::ULongToHexString(static_cast<std::uint64_t>(param3)));
        con->PutString("\nParam4:");
        con->PutString(Library::String::ULongToHexString(static_cast<std::uint64_t>(param4)));
        con->Flush();

        Arch::x86_64::HaltProcessor();
    }
}
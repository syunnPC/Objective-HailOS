#include "ACPIManager.hpp"
#include "StringUtility.hpp"
#include "IO.hpp"
#include "RefPtr.hpp"
#include "Timer.hpp"
#include "Panic.hpp"

namespace Kernel
{
    constexpr auto ACPI_SIG_RSDP = "RSD PTR ";
    constexpr auto ACPI_SIG_XSDT = "XSDT";
    constexpr auto ACPI_SIG_RSDT = "RSDT";
    constexpr auto ACPI_SIG_FACP = "FACP";

    constexpr std::uint16_t SLP_TYPE_S5 = (0x5 << 10);
    constexpr std::uint16_t SLP_EN = (1 << 13);
    constexpr std::uint16_t SLP_CMD = SLP_TYPE_S5 | SLP_EN;

    static inline bool CheckSigEq(const char* sig1, const char* sig2, std::size_t length)
    {
        for (std::size_t i = 0; i < length; ++i)
        {
            if (sig1[i] != sig2[i])
            {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    static inline void WritePhysMem(void* addr, T value)
    {
        if (addr == 0)
        {
            return;
        }

        volatile T* ptr = reinterpret_cast<volatile T*>(addr);
        *ptr = value;
    }

    static bool ValidateRSDPChecksum(RSDPtr* rsdp)
    {
        if (rsdp == nullptr)
        {
            return false;
        }

        if (rsdp->Revision < 2)
        {
            std::uint8_t* p = reinterpret_cast<std::uint8_t*>(rsdp);
            std::uint8_t sum = 0;
            for (size_t i = 0; i < 20; i++)
            {
                sum = static_cast<std::uint8_t>(sum + p[i]);
            }

            return sum == 0;
        }
        else
        {
            std::uint8_t* p = reinterpret_cast<std::uint8_t*>(rsdp);
            std::uint32_t len = rsdp->Length;

            if (len < 20)
            {
                return false;
            }

            std::uint8_t sum = 0;
            for (std::uint32_t i = 0; i < len; i++)
            {
                sum = static_cast<std::uint8_t>(sum + p[i]);
            }

            return sum == 0;
        }
    }

    static SDTHeader* FindTable(RSDPtr* rsdp, const char* sig)
    {
        if (rsdp == nullptr || sig == nullptr)
        {
            return nullptr;
        }

        if (rsdp->Revision >= 2)
        {
            SDTHeader* xsdt = reinterpret_cast<SDTHeader*>(rsdp->XSDTAddress);
            if (xsdt == nullptr)
            {
                return nullptr;
            }

            std::uint32_t entries = (xsdt->Length - sizeof(SDTHeader)) / sizeof(std::uint64_t);
            std::uint64_t* entry = reinterpret_cast<std::uint64_t*>(reinterpret_cast<std::uintptr_t>(xsdt) + sizeof(SDTHeader));
            for (std::uint32_t i = 0; i < entries; i++)
            {
                SDTHeader* h = reinterpret_cast<SDTHeader*>(entry[i]);
                if (h != nullptr && CheckSigEq(h->Signature, sig, Library::String::StringLength(sig)))
                {
                    return h;
                }
            }
        }
        else
        {
            SDTHeader* rsdt = reinterpret_cast<SDTHeader*>(rsdp->RSDTAddress);
            if (rsdt == nullptr)
            {
                return nullptr;
            }

            std::uint32_t entries = (rsdt->Length - sizeof(SDTHeader)) / sizeof(std::uint32_t);
            std::uint32_t* entry = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(rsdt + sizeof(SDTHeader)));
            for (std::uint32_t i = 0; i < entries; i++)
            {
                SDTHeader* h = reinterpret_cast<SDTHeader*>(entry[i]);
                if (h != nullptr && CheckSigEq(h->Signature, sig, Library::String::StringLength(sig)))
                {
                    return h;
                }
            }
        }

        return nullptr;
    }

    ACPIManager::ACPIManager(RSDPtr* ptr) noexcept : m_RSDPtr(ptr)
    {
        if (ptr)
        {
            if (!ValidateRSDPChecksum(ptr))
            {
                m_RSDPtr = nullptr;
            }
        }
    }

    void ACPIManager::Shutdown() noexcept
    {
        if (m_RSDPtr == nullptr)
        {
            PANIC(Status::STATUS_ERROR, 0);
        }

        SDTHeader* fadtHdr = FindTable(m_RSDPtr, ACPI_SIG_FACP);
        if (fadtHdr == nullptr)
        {
            PANIC(Status::STATUS_ERROR, 0);
        }

        FADT* fadt = reinterpret_cast<FADT*>(fadtHdr);
        if (fadt->ACPIEnable != 0 && fadt->SMICommandPort != 0)
        {
            Arch::x86_64::IO::Out8(fadt->SMICommandPort, fadt->ACPIEnable);
        }

        bool mmio = false;
        std::uint64_t pm1a = 0;
        std::size_t width;
        bool bMMIO = false;
        bool bEnable = false;
        std::uint64_t pm1b = 0;
        std::size_t bWidth;

        if (fadt->X_PM1aCtrlBlock.Address != 0)
        {
            mmio = (fadt->X_PM1aCtrlBlock.AddressSpaceID == 0);
            pm1a = fadt->X_PM1aCtrlBlock.Address;
            width = (fadt->X_PM1aCtrlBlock.AccessSize != 0) ? (1 << (fadt->X_PM1aCtrlBlock.AccessSize - 1)) : fadt->PM1CtrlLength;
        }
        else if (fadt->PM1aCtrlBlock != 0)
        {
            mmio = false;
            pm1a = fadt->PM1aCtrlBlock;
            width = fadt->PM1CtrlLength;
        }
        else
        {
            PANIC(Status::STATUS_ERROR, 0);
        }

        if (fadt->X_PM1bCtrlBlock.Address != 0)
        {
            bMMIO = (fadt->X_PM1bCtrlBlock.AddressSpaceID == 0);
            pm1b = fadt->X_PM1bCtrlBlock.Address;
            bWidth = (fadt->X_PM1bCtrlBlock.AccessSize != 0) ? (1 << (fadt->X_PM1bCtrlBlock.AccessSize - 1)) : fadt->PM1CtrlLength;
            bEnable = true;
        }
        else if (fadt->PM1bCtrlBlock != 0)
        {
            bMMIO = false;
            pm1b = fadt->PM1bCtrlBlock;
            bWidth = fadt->PM1CtrlLength;
            bEnable = true;
        }

        if (!mmio)
        {
            Arch::x86_64::IO::Out16(static_cast<std::uint16_t>(pm1a), SLP_CMD);
        }
        else
        {
            WritePhysMem(reinterpret_cast<void*>(pm1a), SLP_CMD);
        }

        if (bEnable)
        {
            if (!bMMIO)
            {
                Arch::x86_64::IO::Out16(static_cast<std::uint16_t>(pm1b), SLP_CMD);
            }
            else
            {
                WritePhysMem(reinterpret_cast<void*>(pm1b), SLP_CMD);
            }
        }

        RefPtr<Timer> t;
        t->Sleep(10, TimeScale::SECONDS);
        asm volatile("int3");
    }
}
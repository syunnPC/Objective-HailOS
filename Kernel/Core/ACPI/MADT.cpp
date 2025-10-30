#include "MADT.hpp"
#include "StringUtility.hpp"

namespace
{
    inline bool CheckSigEq(const char* sig1, const char* sig2, std::size_t length) noexcept
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

    Kernel::ACPI::SDTHeader* FindTable(Kernel::ACPI::RSDPtr* rsdp, const char* sig) noexcept
    {
        if (rsdp == nullptr || sig == nullptr)
        {
            return nullptr;
        }

        if (rsdp->Revision >= 2)
        {
            Kernel::ACPI::SDTHeader* xsdt = reinterpret_cast<Kernel::ACPI::SDTHeader*>(rsdp->XSDTAddress);
            if (xsdt == nullptr)
            {
                return nullptr;
            }

            std::uint32_t entries = (xsdt->Length - sizeof(Kernel::ACPI::SDTHeader)) / sizeof(std::uint64_t);
            std::uint64_t* entry = reinterpret_cast<std::uint64_t*>(reinterpret_cast<std::uintptr_t>(xsdt) + sizeof(Kernel::ACPI::SDTHeader));
            for (std::uint32_t i = 0; i < entries; i++)
            {
                Kernel::ACPI::SDTHeader* h = reinterpret_cast<Kernel::ACPI::SDTHeader*>(entry[i]);
                if (h != nullptr && CheckSigEq(h->Signature, sig, Library::String::StringLength(sig)))
                {
                    return h;
                }
            }
        }
        else
        {
            Kernel::ACPI::SDTHeader* rsdt = reinterpret_cast<Kernel::ACPI::SDTHeader*>(rsdp->RSDTAddress);
            if (rsdt == nullptr)
            {
                return nullptr;
            }

            std::uint32_t entries = (rsdt->Length - sizeof(Kernel::ACPI::SDTHeader)) / sizeof(std::uint32_t);
            std::uint32_t* entry = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(rsdt + sizeof(Kernel::ACPI::SDTHeader)));
            for (std::uint32_t i = 0; i < entries; i++)
            {
                Kernel::ACPI::SDTHeader* h = reinterpret_cast<Kernel::ACPI::SDTHeader*>(entry[i]);
                if (h != nullptr && CheckSigEq(h->Signature, sig, Library::String::StringLength(sig)))
                {
                    return h;
                }
            }
        }

        return nullptr;
    }
}

namespace Kernel::ACPI
{
    bool ParseMADT(RSDPtr* rsdp, ParsedMADT& out) noexcept
    {
        auto* hdr = FindTable(rsdp, ACPI_SIG_MADT);
        if (!hdr)
        {
            return false;
        }

        auto* madt = reinterpret_cast<const MADTHeader*>(hdr);
        out.LocalAPICBase = madt->LocalAPICAddress;

        const std::uint8_t* begin = reinterpret_cast<const std::uint8_t*>(madt) + sizeof(MADTHeader);
        const std::uint8_t* end = reinterpret_cast<const std::uint8_t*>(madt) + madt->Header.Length;

        for (auto* p = begin; p + sizeof(MADTEntryHeader) <= end;)
        {
            auto* eh = reinterpret_cast<const MADTEntryHeader*>(p);
            if (eh->Length == 0)
            {
                break;
            }

            const std::uint8_t* next = p + eh->Length;

            switch (static_cast<MADTEntryType>(eh->Type))
            {
            case MADTEntryType::IOAPIC:
                if (eh->Length >= sizeof(MADTEntryHeader) + sizeof(MADT_IOAPIC) && out.IOAPICCount < sizeof(out.IOAPICs) / sizeof(out.IOAPICs[0]))
                {
                    auto* ioa = reinterpret_cast<const MADT_IOAPIC*>(p + sizeof(MADTEntryHeader));
                    out.IOAPICs[out.IOAPICCount++] = { ioa->IOAPICID, ioa->IOAPICAddress, ioa->GlobalSystemInterruptBase };
                }
                break;
            case MADTEntryType::InterruptSourceOverrride:
                if (eh->Length >= sizeof(MADTEntryHeader) + sizeof(MADT_ISO) && out.ISOCount < sizeof(out.ISOs) / sizeof(out.ISOs[0]))
                {
                    auto* iso = reinterpret_cast<const MADT_ISO*>(p + sizeof(MADTHeader));
                    out.ISOs[out.ISOCount++] = { iso->IRQSource, iso->GlobalSystemInterrupt, iso->Flags };
                }
                break;
            default:
                break;
            }

            p = next;
        }

        return true;
    }
}
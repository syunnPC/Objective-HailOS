#pragma once

#include "ACPIManager.hpp"

namespace Kernel::ACPI
{
    inline constexpr auto ACPI_SIG_MADT = "APIC";

    struct MADTHeader
    {
        SDTHeader Header;
        std::uint32_t LocalAPICAddress;
        std::uint32_t Flags;
    } __attribute__((packed));

    enum class MADTEntryType : std::uint8_t
    {
        ProcessorLocalAPIC = 0,
        IOAPIC = 1,
        InterruptSourceOverrride = 2,
        NMI = 4,
        LocalAPICNMI = 5,
    };

    struct MADTEntryHeader
    {
        std::uint8_t Type;
        std::uint8_t Length;
    } __attribute__((packed));

    struct MADT_IOAPIC
    {
        std::uint8_t IOAPICID;
        std::uint8_t Reserved;
        std::uint32_t IOAPICAddress;
        std::uint32_t GlobalSystemInterruptBase;
    } __attribute__((packed));

    struct MADT_ISO
    {
        std::uint8_t BusSource;
        std::uint8_t IRQSource;
        std::uint32_t GlobalSystemInterrupt;
        std::uint16_t Flags;
    } __attribute__((packed));

    struct ParsedMADT
    {
        std::uint64_t LocalAPICBase = 0;
        std::uint8_t BSP_APIC_ID = 0xFF;

        struct IOAPICInfo
        {
            std::uint8_t ID;
            std::uint32_t Phys;
            std::uint32_t GSIBase;
        } __attribute__((packed)) IOAPICs[8];

        std::size_t IOAPICCount = 0;

        struct ISOInfo
        {
            std::uint8_t IRQ;
            std::uint32_t GSI;
            std::uint16_t Flags;
        } __attribute__((packed)) ISOs[32];

        std::size_t ISOCount = 0;
    };

    bool ParseMADT(RSDPtr* rsdp, ParsedMADT& out) noexcept;
}
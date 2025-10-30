#include "IOAPIC.hpp"

namespace Kernel::Arch::x86_64::APIC
{
    std::uint32_t IOAPIC::Read(std::uint32_t reg) const noexcept
    {
        *Sel() = reg;
        return *Win();
    }

    void IOAPIC::Write(std::uint32_t reg, std::uint32_t val) const noexcept
    {
        *Sel() = reg;
        *Win() = val;
    }

    std::uint32_t IOAPIC::Version() const noexcept
    {
        return Read(0x01);
    }

    std::uint32_t IOAPIC::RedirCount() const noexcept
    {
        return ((Version() >> 16) & 0xFFu);
    }

    std::uint64_t IOAPIC::ReadRedir(std::uint32_t idx) const noexcept
    {
        const std::uint32_t regLo = 0x10 + idx * 2;
        const std::uint32_t regHi = regLo + 1;
        std::uint64_t lo = Read(regLo);
        std::uint64_t hi = Read(regHi);
        return lo | (hi << 32);
    }

    void IOAPIC::WriteRedir(std::uint32_t idx, std::uint64_t val) const noexcept
    {
        const std::uint32_t regLo = 0x10 + idx * 2;
        const std::uint32_t regHi = regLo + 1;
        Write(regHi, static_cast<std::uint32_t>(val >> 32));
        Write(regLo, static_cast<std::uint32_t>(val & 0xFFFFFFFFu));
    }

    bool IOAPIC::OwnsGSI(std::uint32_t gsi) const noexcept
    {
        const std::uint32_t max = RedirCount() + 1u;
        return (gsi >= m_GSIBase) && (gsi < m_GSIBase + max);
    }

    void IOAPIC::RouteGSI(std::uint32_t gsi, std::uint8_t vector, std::uint8_t destAPICID, std::uint8_t polarity, std::uint8_t trigger, bool masked) noexcept
    {
        if (!OwnsGSI(gsi))
        {
            return;
        }

        const std::uint32_t idx = gsi - m_GSIBase;

        std::uint32_t lo = 0;
        lo |= vector;

        if (polarity)
        {
            lo |= (1u << 13);
        }

        if (trigger)
        {
            lo |= (1u << 15);
        }

        if (masked)
        {
            lo |= (1u << 16);
        }

        std::uint32_t hi = static_cast<std::uint32_t>(destAPICID) << 24;
        WriteRedir(idx, (static_cast<std::uint64_t>(hi) << 32) | lo);
    }
}
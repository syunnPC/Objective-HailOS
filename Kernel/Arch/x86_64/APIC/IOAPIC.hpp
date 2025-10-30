#pragma once

#include "IKernelObject.hpp"
#include "RefCountedBase.hpp"

namespace Kernel::Arch::x86_64::APIC
{
    class IOAPIC : public virtual IKernelObject, public RefCountedBase
    {
    private:
        std::uint32_t m_Base;
        std::uint32_t m_GSIBase;
    public:
        IOAPIC(std::uint32_t physBase, std::uint32_t gsiBase) noexcept : m_Base(physBase), m_GSIBase(gsiBase) {}
        std::uint32_t Version() const noexcept;
        std::uint32_t RedirCount() const noexcept;

        void RouteGSI(std::uint32_t gsi, std::uint8_t vector, std::uint8_t destAPICID, std::uint8_t polarity, std::uint8_t trigger, bool masked = false) noexcept;
        bool OwnsGSI(std::uint32_t gsi) const noexcept;
        inline std::uint32_t GetGSIBase() const noexcept
        {
            return m_GSIBase;
        }
    protected:
        virtual ~IOAPIC() noexcept override = default;
    private:
        inline volatile std::uint32_t* Sel() const noexcept
        {
            return reinterpret_cast<volatile std::uint32_t*>(static_cast<std::uintptr_t>(m_Base));
        }

        inline volatile std::uint32_t* Win() const noexcept
        {
            return reinterpret_cast<volatile std::uint32_t*>(static_cast<std::uintptr_t>(m_Base + 0x10));
        }

        std::uint32_t Read(std::uint32_t reg) const noexcept;
        void Write(std::uint32_t reg, std::uint32_t val) const noexcept;
        std::uint64_t ReadRedir(std::uint32_t idx) const noexcept;
        void WriteRedir(std::uint32_t idx, std::uint64_t val) const noexcept;
    };
}
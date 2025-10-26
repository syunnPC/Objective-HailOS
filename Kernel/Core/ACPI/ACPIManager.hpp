#pragma once

#include "IPowerManager.hpp"
#include "RefCountedBase.hpp"

namespace Kernel
{
    struct SDTHeader
    {
        char Signature[4];
        std::uint32_t Length;
        std::uint8_t Revision;
        std::uint8_t Checksum;
        char OEMID[6];
        char OEMTableID[8];
        std::uint32_t OEMRevision;
        std::uint32_t CreatorID;
        std::uint32_t CreatorRev;
    } __attribute__((packed));

    struct RSDPtr
    {
        char Signature[8];
        std::uint8_t Checksum;
        char OEMID[6];
        std::uint8_t Revision;
        std::uint32_t RSDTAddress;
        std::uint32_t Length;
        std::uint64_t XSDTAddress;
        std::uint8_t ExtendedChecksum;
        std::uint8_t Reserved[3];
    } __attribute__((packed));

    struct GenericAddress
    {
        std::uint8_t AddressSpaceID;
        std::uint8_t RegisterBitWidth;
        std::uint8_t RegisterBitOffset;
        std::uint8_t AccessSize;
        std::uint64_t Address;
    } __attribute__((packed));

    struct FADT
    {
        SDTHeader Header;
        std::uint32_t FirmwareCtrl;
        std::uint32_t DSDT;
        std::uint8_t Reserved1;
        std::uint8_t PreferredPowerProfile;
        std::uint16_t SCIInterrupt;
        std::uint32_t SMICommandPort;
        std::uint8_t ACPIEnable;
        std::uint8_t ACPIDisable;
        std::uint8_t S4BIOSReq;
        std::uint8_t PStateCtrl;
        std::uint32_t PM1aEventBlock;
        std::uint32_t PM1bEventBlock;
        std::uint32_t PM1aCtrlBlock;
        std::uint32_t PM1bCtrlBlock;
        std::uint32_t PM2CtrlBlock;
        std::uint32_t PMTimerBlock;
        std::uint32_t GPE0Block;
        std::uint32_t GPE1Block;
        std::uint8_t PM1EventLength;
        std::uint8_t PM1CtrlLength;
        std::uint8_t PM2CtrlLength;
        std::uint8_t PMTimerLength;
        std::uint8_t GPE0Length;
        std::uint8_t GPE1Length;
        std::uint8_t GPE1Base;
        std::uint8_t CStateCtrl;
        std::uint16_t WorstC2Latency;
        std::uint16_t WorstC3Latency;
        std::uint16_t FlushSize;
        std::uint16_t FlushStride;
        std::uint8_t DutyOffset;
        std::uint8_t DutyWidth;
        std::uint8_t DayAlarm;
        std::uint8_t MonthAlarm;
        std::uint8_t Century;
        std::uint16_t BootArchitectureFlags;
        std::uint8_t Reserved2;
        std::uint32_t Flags;
        GenericAddress ResetReg;
        std::uint8_t ReserValue;
        std::uint8_t Reserved3[3];
        std::uint64_t X_FirmwareCtrl;
        std::uint64_t X_DSDT;
        GenericAddress X_PM1aEventBlock;
        GenericAddress X_PM1bEventBlock;
        GenericAddress X_PM1aCtrlBlock;
        GenericAddress X_PM1bCtrlBlock;
        GenericAddress X_PM2CtrlBlock;
        GenericAddress X_PMTimerBlock;
        GenericAddress X_GPE0Block;
        GenericAddress X_GPE1Block;
    } __attribute__((packed));

    class ACPIManager final : public RefCountedBase, public virtual IPowerManager
    {
    public:
        ACPIManager() = delete;
        ACPIManager(const ACPIManager&) = delete;
        ACPIManager(ACPIManager&&) noexcept = default;
        ACPIManager(RSDPtr* ptr) noexcept;
        virtual void Shutdown() noexcept override;
    protected:
        virtual ~ACPIManager() noexcept override = default;
    private:
        RSDPtr* m_RSDPtr;
    };
}
#pragma once

#include "IDevice.hpp"

namespace Kernel
{
    struct PCILocation
    {
        uint8_t Bus;
        uint8_t Device;
        uint8_t Function;
    };

    struct PCIDeviceFeature
    {
        uint8_t Class;
        uint8_t SubClass;
        uint8_t ProgIf;
    };

    inline constexpr InterfaceID IID_IPCIDevice{ {
        0x3A, 0xA2, 0x38, 0xE1, 0xD1, 0x36, 0x69, 0x27, 0x26, 0x79, 0x5B, 0x4B, 0x04, 0x0D, 0x0D, 0x70
    } };

    class IPCIDevice : public virtual IDevice
    {
    public:
        virtual PCILocation GetPCILocation() const noexcept = 0;
        virtual PCIDeviceFeature GetPCIDeviceFeature() const noexcept = 0;
        virtual std::uintptr_t GetBARAddress(int bar) const noexcept = 0;
        virtual uint16_t GetVendorID() const noexcept = 0;
        virtual uint16_t GetDeviceID() const noexcept = 0;
    protected:
        virtual ~IPCIDevice() override = default;
    };
}
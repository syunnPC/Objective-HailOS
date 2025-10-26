#pragma once

#include "IKernelObject.hpp"

namespace Kernel
{
    enum class DeviceTypeMajor : std::uint16_t
    {
        UNKNOWN = 0,
        BRIDGE,
        CONTROLLER,
        STORAGE,
        NETWORK,
        DISPLAY,
        AUDIO,
        INPUT,
        OUTPUT,
        CAMERA,
        COMMUNICATION,
        POWER,
        TIMER,
        SECURITY,
        MISC = 0xFFFF,
    };

    enum class DeviceBusType : std::uint16_t
    {
        UNKNOWN,
        PCI,
        USB,
        UART,
    };

    constexpr std::uint32_t MakeDeviceType(DeviceTypeMajor major, std::uint16_t minor)
    {
        return ((static_cast<std::uint32_t>(major) << 16) | minor);
    }

    enum class DeviceType : std::uint32_t
    {
        UNKNOWN = MakeDeviceType(DeviceTypeMajor::UNKNOWN, 0),

        USB_CONTROLLER_XHCI = MakeDeviceType(DeviceTypeMajor::CONTROLLER, 0x0101),
        USB_CONTROLLER_EHCI = MakeDeviceType(DeviceTypeMajor::CONTROLLER, 0x0102),
        AHCI_CONTROLLER = MakeDeviceType(DeviceTypeMajor::CONTROLLER, 0x0201),
        IDE_CONTROLLER = MakeDeviceType(DeviceTypeMajor::CONTROLLER, 0x0202),

        SATA_DISK = MakeDeviceType(DeviceTypeMajor::STORAGE, 0x0001),
        IDE_DISK = MakeDeviceType(DeviceTypeMajor::STORAGE, 0x0002),

        PS2_MOUSE = MakeDeviceType(DeviceTypeMajor::INPUT, 0x0101),
        HID_MOUSE = MakeDeviceType(DeviceTypeMajor::INPUT, 0x0201),
        PS2_KEYBOARD = MakeDeviceType(DeviceTypeMajor::INPUT, 0x0102),
        HID_KEYBOARD = MakeDeviceType(DeviceTypeMajor::INPUT, 0x0202),

        HW_RANDOM_ENGINE = MakeDeviceType(DeviceTypeMajor::SECURITY, 0x0001),

        CONSOLE = MakeDeviceType(DeviceTypeMajor::OUTPUT, 0x0000),
    };

    inline constexpr InterfaceID IID_IDevice{ {
        0x57, 0xF7, 0xAE, 0x15, 0xD0, 0x1C, 0x4B, 0xE1, 0x88, 0xDD, 0xB5, 0x88, 0x42, 0x5D, 0x95, 0xE5
    } };

    class IDevice : public virtual IKernelObject
    {
    public:
        virtual DeviceType GetDeviceType() const noexcept = 0;
        virtual void Reset() noexcept = 0;
        virtual void Shutdown() noexcept = 0;
    protected:
        virtual ~IDevice() override = default;
    };
}
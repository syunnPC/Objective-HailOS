#pragma once

#include "RefCountedBase.hpp"
#include "IRandomDevice.hpp"

namespace Kernel
{
    class HardwareRandomDevice : public RefCountedBase, public virtual IRandomDevice
    {
    public:
        HardwareRandomDevice() noexcept = default;
        HardwareRandomDevice(const HardwareRandomDevice&) noexcept = default;
        HardwareRandomDevice(HardwareRandomDevice&&) noexcept = default;
        virtual std::uint16_t Generate16() noexcept override;
        virtual std::uint32_t Generate32() noexcept override;
        virtual std::uint64_t Generate64() noexcept override;
        virtual DeviceType GetDeviceType() const noexcept override;
        virtual void Reset() noexcept override;
        virtual void Shutdown() noexcept override;
    protected:
        virtual ~HardwareRandomDevice() override = default;
    private:
        DeviceType m_DeviceType{ DeviceType::HW_RANDOM_ENGINE };
    };
}
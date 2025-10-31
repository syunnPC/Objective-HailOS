#include "HardwareRandomDevice.hpp"
#include "Arch/x86_64/Utility/Random.hpp"

namespace Kernel
{
    /*
        現在はx86-64（RDRAND）のみ対応
        将来的にはマクロでコンパイル時に変更
    */

    std::uint16_t HardwareRandomDevice::Generate16() noexcept
    {
        return Arch::x86_64::HwGenerateRandom16();
    }

    std::uint32_t HardwareRandomDevice::Generate32() noexcept
    {
        return Arch::x86_64::HwGenerateRandom32();
    }

    std::uint64_t HardwareRandomDevice::Generate64() noexcept
    {
        return Arch::x86_64::HwGenerateRandom64();
    }

    DeviceType HardwareRandomDevice::GetDeviceType() const noexcept
    {
        return m_DeviceType;
    }

    void HardwareRandomDevice::Reset() noexcept
    {
        return;
    }

    void HardwareRandomDevice::Shutdown() noexcept
    {
        return;
    }
}
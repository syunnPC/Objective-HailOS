#pragma once

#include "IDevice.hpp"

namespace Kernel
{
    inline constexpr InterfaceID IID_ITimerDevice{ {
        0x11, 0xA4, 0xDB, 0xD0, 0x93, 0x55, 0x1F, 0x0E, 0x02, 0xA4, 0x68, 0x50, 0xA5, 0xB1, 0x0A, 0xC0
    } };

    enum class TimeScale
    {
        SECONDS = 1,
        MILLISECONDS = 1000,
        NANOSECOUNDS = 1000000000,
    };

    class ITimer : public virtual IKernelObject
    {
    public:
        [[nodiscard]] virtual std::uint64_t GetFrequency() const noexcept = 0;
        [[nodiscard]] virtual std::uint64_t GetCurrentStamp() const noexcept = 0;
        virtual void Sleep(std::uint64_t value, TimeScale timeScale) noexcept = 0;
    protected:
        virtual ~ITimer() noexcept override = default;
    };
}
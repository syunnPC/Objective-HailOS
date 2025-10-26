#pragma once

#include "ITimer.hpp"
#include "RefCountedBase.hpp"

namespace Kernel
{
    struct HardwareClockInfo
    {
        std::uint64_t InitialUNIXTime;
        std::uint64_t InitialTSC;
        std::uint64_t TSCFreq;
    } __attribute__((packed));

    /// @brief TSC周波数などを記録する、起動直後にmainから呼ぶ
    /// @param info ブートローダーから渡されたTSCクロック情報
    void SetHardwareTimerInfo(HardwareClockInfo* info);

    enum class TimerType
    {
        TSC_TIMER,
        APIC_TIMER,
    };

    /*
        TODO: APICタイマーなどを実装
        当面はTSCのみで実装
        LapTimerとStopTimerの違いは今のところないのでどっちを呼んでも変わらない
        将来まともに実装するときには変わるかもしれないので、止める場合はStopTimer()、続ける場合はLapTimer()を呼ぶこと.
    */
    class Timer final : public virtual ITimer, public RefCountedBase
    {
    public:
        Timer() noexcept;
        Timer(TimerType type) noexcept;
        Timer(const Timer&) noexcept = default;
        Timer(Timer&&) noexcept = default;

        [[nodiscard]] virtual std::uint64_t GetFrequency() const noexcept override;
        [[nodiscard]] virtual std::uint64_t GetCurrentStamp() const noexcept override;
        virtual void Sleep(std::uint64_t value, TimeScale timeScale) noexcept override;
        [[nodiscard]] TimerType GetTimerType() const noexcept;
        void StartTimer() noexcept;
        void RestartTimer() noexcept;
        void SetTimeScale(TimeScale timeScale) noexcept;
        [[nodiscard]] TimeScale GetTimeScale() noexcept;
        [[nodiscard]] std::uint64_t StopTimer(TimeScale timeScale) noexcept;
        [[nodiscard]] std::uint64_t LapTimer(TimeScale timeScale) noexcept;
        [[nodiscard]] std::uint64_t StopTimer() noexcept;
        [[nodiscard]] std::uint64_t LapTimer() noexcept;
        void Sleep(std::uint64_t value) noexcept;
    protected:
        virtual ~Timer() noexcept override = default;
    private:
        std::uint64_t m_Freq;
        std::uint64_t m_LastStamp;
        TimeScale m_TimeScale;
        TimerType m_Type;
    };
}
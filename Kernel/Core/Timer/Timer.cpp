#include "Timer.hpp"
#include "TSC.hpp"

namespace Kernel
{
    static HardwareClockInfo* gHwClockInfo;

    void SetHardwareTimerInfo(HardwareClockInfo* info)
    {
        gHwClockInfo = info;
    }

    Timer::Timer() noexcept : m_Freq(gHwClockInfo->TSCFreq), m_LastStamp(0), m_TimeScale(TimeScale::MILLISECONDS), m_Type(TimerType::TSC_TIMER) {}

    //ここで指定しても未実装なのでフル無視でTSCタイマー
    Timer::Timer(TimerType type) noexcept : m_LastStamp(0), m_TimeScale(TimeScale::MILLISECONDS), m_Type(type)
    {
        if (type != TimerType::TSC_TIMER)
        {
            m_Type = TimerType::TSC_TIMER;
        }

        m_Freq = gHwClockInfo->TSCFreq;
    }

    [[nodiscard]] std::uint64_t Timer::GetFrequency() const noexcept
    {
        return m_Freq;
    }

    [[nodiscard]] TimerType Timer::GetTimerType() const noexcept
    {
        return m_Type;
    }

    [[nodiscard]] std::uint64_t Timer::GetCurrentStamp() const noexcept
    {
        //現時点ではこれ以外存在しないはず
        if (m_Type == TimerType::TSC_TIMER)
        {
            return Arch::x86_64::ReadTSC();
        }

        //Unreachableだが返す
        return 0;
    }

    void Timer::Sleep(std::uint64_t value, TimeScale timeScale) noexcept
    {
        if (m_Type == TimerType::TSC_TIMER)
        {
            auto begin = Arch::x86_64::ReadTSC();
            double sleepSec = value / static_cast<std::int64_t>(timeScale);
            std::uint64_t waitClock = static_cast<std::uint64_t>(m_Freq * sleepSec);
            while (Arch::x86_64::ReadTSC() < begin + waitClock);
            return;
        }
    }

    void Timer::Sleep(std::uint64_t value) noexcept
    {
        return Sleep(value, m_TimeScale);
    }

    void Timer::SetTimeScale(TimeScale timeScale) noexcept
    {
        m_TimeScale = timeScale;
    }

    void Timer::StartTimer() noexcept
    {
        if (m_Type == TimerType::TSC_TIMER)
        {
            m_LastStamp = Arch::x86_64::ReadTSC();
            return;
        }
    }

    void Timer::RestartTimer() noexcept
    {
        if (m_Type == TimerType::TSC_TIMER)
        {
            m_LastStamp = Arch::x86_64::ReadTSC();
            return;
        }
    }

    [[nodiscard]] TimeScale Timer::GetTimeScale() noexcept
    {
        return m_TimeScale;
    }

    [[nodiscard]] std::uint64_t Timer::StopTimer(TimeScale timeScale) noexcept
    {
        if (m_Type == TimerType::TSC_TIMER)
        {
            auto delta = Arch::x86_64::ReadTSC() - m_LastStamp;
            double elapsed = delta / m_Freq;
            return static_cast<std::uint64_t>(elapsed * static_cast<std::uint64_t>(timeScale));
        }

        //Unreachableだが返す
        return 0;
    }

    [[nodiscard]] std::uint64_t Timer::LapTimer(TimeScale timeScale) noexcept
    {
        if (m_Type == TimerType::TSC_TIMER)
        {
            auto delta = Arch::x86_64::ReadTSC() - m_LastStamp;
            double elapsed = delta / m_Freq;
            return static_cast<std::uint64_t>(elapsed * static_cast<std::uint64_t>(timeScale));
        }

        //Unreachableだが返す
        return 0;
    }

    [[nodiscard]] std::uint64_t Timer::StopTimer() noexcept
    {
        return StopTimer(m_TimeScale);
    }

    [[nodiscard]] std::uint64_t Timer::LapTimer() noexcept
    {
        return LapTimer(m_TimeScale);
    }
}
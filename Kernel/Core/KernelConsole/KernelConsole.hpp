#pragma once

#include "IOutputDevice.hpp"
#include "RefCountedBase.hpp"
#include "RefPtr.hpp"
#include "SerialConsole.hpp"
#include "RedundantOutput.hpp"

namespace Kernel::Early
{
    /*
        カーネル用コンソール
        起動時・パニック時に利用
        フレームバッファ直書きのため大量のログは不可
    */

    enum class PixelFormat
    {
        BGR,
        RGB,
        INVALID,
    };

    struct GraphicInfo
    {
        std::uintptr_t FrameBufferBase;
        std::size_t FrameBufferSize;
        std::uint32_t PixelsPerScanLine;
        std::uint32_t HorizontalResolution;
        std::uint32_t VerticalResolution;
        PixelFormat DisplayPixelFormat;
    } __attribute__((packed));


    class KernelConsole final : public virtual IOutputDevice, public RefCountedBase
    {
    public:
        KernelConsole() noexcept = delete;
        KernelConsole(const KernelConsole&) noexcept = delete;
        KernelConsole(KernelConsole&&) noexcept = delete;
        KernelConsole(GraphicInfo* info) noexcept;

        virtual void PutChar(char ch) noexcept override;
        virtual void PutString(const char* str) noexcept override;
        virtual void Flush() noexcept override;
        virtual DeviceType GetDeviceType() const noexcept override;
        virtual void Reset() noexcept override;
        virtual void Shutdown() noexcept override;

    protected:
        virtual ~KernelConsole() noexcept override = default;
    private:

        struct FrameBufferColor
        {
            std::uint8_t Color1;
            std::uint8_t Color2;
            std::uint8_t Color3;
            std::uint8_t Color4;
        } __attribute__((packed));

        struct Point
        {
            std::uint32_t X;
            std::uint32_t Y;
        };

        const FrameBufferColor COLOR_BACKGROUND{ 0, 0, 0, 0 };
        const FrameBufferColor COLOR_FOREGROUND{ 255, 255, 255, 0 };

        const GraphicInfo* m_Info;
        Point m_CursorPosition;

        bool IsNewLineRequired() const noexcept;
        bool IsEndOfScreen() const noexcept;
        void DrawPixel(Point location, FrameBufferColor color) noexcept;
        std::size_t CalcPixelOffset(KernelConsole::Point point) const noexcept;
    };

    inline RefPtr<KernelConsole>& ConsoleOwner() noexcept
    {
        static RefPtr<KernelConsole> owner;
        return owner;
    }

    inline void InitKernelConsole(GraphicInfo* info) noexcept
    {
        auto& owner = ConsoleOwner();
        if (!owner)
        {
            owner = RefPtr<KernelConsole>(new KernelConsole(info));
        }
    }

    inline RefPtr<IOutputDevice> GetKernelConsole() noexcept
    {
        auto& owner = ConsoleOwner();
        return RefPtr<IOutputDevice>(static_cast<IOutputDevice*>(owner.Get()));
    }

    inline RefPtr<Devices::SerialConsole>& SerialOwner() noexcept
    {
        static RefPtr<Devices::SerialConsole> owner;
        return owner;
    }

    inline void InitSerialConsole() noexcept
    {
        auto& owner = SerialOwner();
        if (!owner)
        {
            owner = RefPtr<Devices::SerialConsole>(new Devices::SerialConsole());
        }
    }

    inline RefPtr<IOutputDevice> GetSerialConsole() noexcept
    {
        auto& owner = SerialOwner();
        return RefPtr<IOutputDevice>(static_cast<IOutputDevice*>(owner.Get()));
    }

    inline RefPtr<IOutputDevice> GetBootstrapConsole() noexcept
    {
        auto& s = SerialOwner();
        auto& c = ConsoleOwner();
        return RefPtr<IOutputDevice>(static_cast<IOutputDevice*>(new Devices::RedundantOutput(s.Get(), c.Get())));
    }
}
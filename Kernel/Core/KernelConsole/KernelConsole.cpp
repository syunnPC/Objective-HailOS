#include "KernelConsole.hpp"
#include "MemoryUtility.hpp"

namespace
{
    inline bool IsVisibleChar(char ch)
    {
        return ch > 0x19 && ch < 0x7f ? true : false;
    }

    Kernel::Early::KernelConsole* gKConsole;
}

namespace Kernel::Early
{
    extern "C" const unsigned char gConsoleFont[127][16];
    constexpr std::uint32_t FONT_HEIGHT = 16;
    constexpr std::uint32_t FONT_WIDTH = 8;

    KernelConsole::KernelConsole(GraphicInfo* info) noexcept : m_Info(info), m_CursorPosition({ 0, 0 })
    {
        Library::Memory::FillMemory(reinterpret_cast<void*>(m_Info->FrameBufferBase), m_Info->FrameBufferSize, 0x00);
    }

    bool KernelConsole::IsNewLineRequired() const noexcept
    {
        if (m_CursorPosition.X > m_Info->HorizontalResolution - FONT_WIDTH)
        {
            return true;
        }

        return false;
    }

    bool KernelConsole::IsEndOfScreen() const noexcept
    {
        if (m_CursorPosition.Y > m_Info->VerticalResolution - FONT_HEIGHT)
        {
            return true;
        }

        return false;
    }

    std::size_t KernelConsole::CalcPixelOffset(Point point) const noexcept
    {
        return ((point.Y * m_Info->PixelsPerScanLine + point.X) * sizeof(FrameBufferColor));
    }

    void KernelConsole::DrawPixel(Point location, FrameBufferColor color) noexcept
    {
        if (location.X >= m_Info->HorizontalResolution || location.Y >= m_Info->VerticalResolution) [[unlikely]]
        {
            return;
        }

        uintptr_t offset = CalcPixelOffset(location);
        FrameBufferColor* p = reinterpret_cast<FrameBufferColor*>(m_Info->FrameBufferBase + offset);
        *p = color;
    }

    void KernelConsole::PutChar(char ch) noexcept
    {
        if (IsVisibleChar(ch)) [[likely]]
        {
            const unsigned char* font = gConsoleFont[static_cast<int>(ch)];
            if (IsNewLineRequired()) [[unlikely]]
            {
                m_CursorPosition.X = 0;
                m_CursorPosition.Y += FONT_HEIGHT;
            }
            if (IsEndOfScreen()) [[unlikely]]
            {
                Reset();
            }

            if (ch == ' ') [[unlikely]]
            {
                m_CursorPosition.X += FONT_WIDTH;
                return;
            }

            for (auto i = 0; i < FONT_HEIGHT; ++i)
            {
                const unsigned char line = font[i];
                for (auto k = 0; k < FONT_WIDTH; ++k)
                {
                    if (line & (1 << (7 - k)))
                    {
                        DrawPixel(Point{ m_CursorPosition.X + k, m_CursorPosition.Y + i }, COLOR_WHITE);
                    }
                    else
                    {
                        DrawPixel(Point{ m_CursorPosition.X + k, m_CursorPosition.Y + i }, COLOR_BLACK);
                    }
                }
            }

            m_CursorPosition.X += FONT_WIDTH;
        }
        else
        {
            switch (ch)
            {
            case 'n':
                m_CursorPosition.Y += FONT_HEIGHT;
                m_CursorPosition.X = 0;
                if (IsEndOfScreen())
                {
                    Reset();
                }
                return;
            default:
                return;
            }
        }
    }

    void KernelConsole::PutString(const char* str) noexcept
    {
        for (int i = 0; str[i] != '\0'; ++i)
        {
            PutChar(str[i]);
        }
    }

    void KernelConsole::Shutdown() noexcept
    {
        return;
    }

    void KernelConsole::Reset() noexcept
    {
        Library::Memory::FillMemory(reinterpret_cast<void*>(m_Info->FrameBufferBase), m_Info->FrameBufferSize, 0x00);
        m_CursorPosition.X = 0;
        m_CursorPosition.Y = 0;
    }

    void KernelConsole::Flush() noexcept
    {
        PutChar('\n');
    }

    DeviceType KernelConsole::GetDeviceType() const noexcept
    {
        return DeviceType::CONSOLE;
    }
}
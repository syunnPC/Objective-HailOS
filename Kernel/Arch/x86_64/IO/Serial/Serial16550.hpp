#pragma once

#include "SpinLock.hpp"
#include "PortIO.hpp"

namespace Kernel::Arch::x86_64::IO::Serial
{
    class Serial16550 final
    {
    private:
        std::uint64_t m_Base;
        Sync::TicketSpinLock m_SpinLock;
    private:
        inline static bool Probe(std::uint16_t base) noexcept
        {
            if (In8(base + 5) == 0xFF)
            {
                return false;
            }

            auto ier = In8(base + 1);
            auto mcr = In8(base + 4);

            Out8(base + 4, mcr | 0x10);

            Out8(base, 0xAE);
            auto r = In8(base);

            Out8(base + 4, mcr);
            Out8(base + 1, ier);

            return r == 0xAE;
        }

    public:
        [[deprecated("The serial base may be depend on each machine")]] Serial16550(std::uint16_t base, std::uint32_t baud) noexcept : m_Base(base)
        {
            Out8(base + 1, 0x00);

            Out8(base + 3, 0x80);
            const std::uint16_t div = static_cast<std::uint16_t>(115200u / (baud ? baud : 115200u));
            Out8(base, static_cast<std::uint8_t>(div & 0xFF));
            Out8(base + 1, static_cast<std::uint8_t>((div >> 8) & 0xFF));

            Out8(base + 3, 0x03);
            Out8(base + 2, 0xC7);
            Out8(base + 4, 0x0B);

            (void)In8(base + 5);
            (void)In8(base + 6);
            (void)In8(base + 2);
        }

        explicit Serial16550(std::uint32_t baud) noexcept
        {
            static constexpr std::uint16_t baseCand[] = { 0x3F8, 0x2F8, 0x3E8, 0x2E8 };
            std::uint16_t base;
            for (auto b : baseCand)
            {
                if (Probe(b))
                {
                    m_Base = b;
                    base = b;
                }
            }

            Out8(base + 1, 0x00);

            Out8(base + 3, 0x80);
            const std::uint16_t div = static_cast<std::uint16_t>(115200u / (baud ? baud : 115200u));
            Out8(base, static_cast<std::uint8_t>(div & 0xFF));
            Out8(base + 1, static_cast<std::uint8_t>((div >> 8) & 0xFF));

            Out8(base + 3, 0x03);
            Out8(base + 2, 0xC7);
            Out8(base + 4, 0x0B);

            (void)In8(base + 5);
            (void)In8(base + 6);
            (void)In8(base + 2);
        }

        inline bool CanTx() const noexcept
        {
            return (In8(m_Base + 5) & 0x20) != 0;
        }

        inline void WriteByte(std::uint8_t v) noexcept
        {
            while (!CanTx()) Sync::CPURelax();
            IO::Out8(m_Base, v);
        }

        inline void Write(const char* s) noexcept
        {
            auto guard = Sync::LockIRQSave(m_SpinLock);
            for (auto p = s; *p != '\0'; ++p)
            {
                if (*p == '\n')
                {
                    WriteByte('\r');
                }
                WriteByte(*p);
            }
            Sync::UnlockIRQRestor(guard);
        }

        inline void Flush() noexcept
        {
            while (!CanTx())
            {
                Sync::CPURelax();
            }
        }

        inline std::uint16_t BasePort() const noexcept
        {
            return m_Base;
        }
    };
}
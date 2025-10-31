#pragma once

#include "IOutputDevice.hpp"
#include "RefCountedBase.hpp"
#include "Serial16550.hpp"

#include <new>

namespace Kernel::Devices
{
    class SerialConsole final : public virtual IOutputDevice, public RefCountedBase
    {
    private:
        Arch::x86_64::IO::Serial::Serial16550* m_Port;
    public:
        SerialConsole() noexcept
        {
            m_Port = new Arch::x86_64::IO::Serial::Serial16550(115200);
        }

        inline virtual void PutChar(char ch) noexcept override
        {
            if (!m_Port)
            {
                return;
            }

            if (ch == '\n')
            {
                m_Port->WriteByte('\r');
            }
            m_Port->WriteByte(ch);
        }

        inline virtual void PutString(const char* str) noexcept override
        {
            if (m_Port)
            {
                m_Port->Write(str);
            }
        }

        inline virtual void Flush() noexcept override
        {
            if (m_Port)
            {
                m_Port->Flush();
            }
        }

        virtual DeviceType GetDeviceType() const noexcept override
        {
            return DeviceType::SERIAL;
        }

        virtual void Reset() noexcept override {}
        virtual void Shutdown() noexcept override {}
    protected:
        virtual ~SerialConsole() noexcept override = default;
    };
}
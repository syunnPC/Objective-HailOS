#pragma once

#include "IOutputDevice.hpp"
#include "RefCountedBase.hpp"

namespace Kernel::Devices
{
    class RedundantOutput final : public virtual IOutputDevice, public RefCountedBase
    {
    private:
        IOutputDevice* m_A;
        IOutputDevice* m_B;
    public:
        RedundantOutput(IOutputDevice* a, IOutputDevice* b) noexcept : m_A(a), m_B(b) {}
        inline virtual void PutChar(char ch) noexcept override
        {
            if (m_A)
            {
                m_A->PutChar(ch);
            }

            if (m_B)
            {
                m_B->PutChar(ch);
            }
        }

        inline virtual void PutString(const char* s) noexcept override
        {
            if (m_A)
            {
                m_A->PutString(s);
            }

            if (m_B)
            {
                m_B->PutString(s);
            }
        }

        inline virtual void Flush() noexcept override
        {
            if (m_A)
            {
                m_A->Flush();
            }

            if (m_B)
            {
                m_B->Flush();
            }
        }

        inline virtual DeviceType GetDeviceType() const noexcept override
        {
            return DeviceType::CONSOLE;
        }

        inline virtual void Reset() noexcept override
        {
            if (m_A)
            {
                m_A->Reset();
            }

            if (m_B)
            {
                m_B->Reset();
            }
        }

        inline virtual void Shutdown() noexcept override
        {
            if (m_A)
            {
                m_A->Shutdown();
            }

            if (m_B)
            {
                m_B->Shutdown();
            }
        }
    };
}
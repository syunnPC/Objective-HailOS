#pragma once

#include <atomic>

#include "IKernelObject.hpp"

namespace Kernel
{
    class RefCountedBase : public virtual IKernelObject
    {
    private:
        std::atomic_size_t m_RefCount{ 1 };
    public:
        std::size_t AddRef() noexcept override
        {
            return m_RefCount.fetch_add(1, std::memory_order_relaxed) + 1;
        }

        void Release() noexcept override
        {
            if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                //TODO:メモリ管理実装後にdelete this;
            }
        }

        bool QueryInterface(const InterfaceID& iid, void** out) noexcept override
        {
            if (!out)
            {
                return false;
            }

            if (iid == IID_IKernelObject)
            {
                *out = static_cast<IKernelObject*>(this);
                AddRef();
                return true;
            }
            *out = nullptr;
            return false;
        }

        RefCountedBase(const RefCountedBase&) = delete;
        RefCountedBase& operator=(const RefCountedBase&) = delete;

    protected:
        RefCountedBase() = default;
        virtual ~RefCountedBase() = default;
    };
}
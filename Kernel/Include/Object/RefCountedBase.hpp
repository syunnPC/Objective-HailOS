#pragma once

#include "IKernelObject.hpp"
#include "NewDelete.hpp"

namespace Kernel
{
    class RefCountedBase : public virtual IKernelObject
    {
    private:
        std::size_t m_RefCount{ 1 };
    public:
        std::size_t AddRef() noexcept override
        {
            return __atomic_fetch_add(&m_RefCount, 1u, __ATOMIC_RELAXED) + 1u;
        }

        void Release() noexcept override
        {
            if (__atomic_fetch_sub(&m_RefCount, 1u, __ATOMIC_ACQ_REL) == 1u)
            {
                __atomic_thread_fence(__ATOMIC_ACQUIRE);
                delete this;
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
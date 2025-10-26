#pragma once

#include "BaseType.hpp"

namespace Kernel
{
    template <class T>
    class RefPtr
    {
    private:
        T* m_Ptr;

    public:
        constexpr RefPtr() noexcept : m_Ptr(nullptr) {}
        constexpr RefPtr(std::nullptr_t) noexcept : m_Ptr(nullptr) {}

        explicit RefPtr(T* ptr) noexcept : m_Ptr(ptr)
        {
            if (m_Ptr)
            {
                m_Ptr->AddRef();
            }
        }

        RefPtr(const RefPtr& v) noexcept : m_Ptr(v.m_Ptr)
        {
            if (m_Ptr)
            {
                m_Ptr->AddRef();
            }
        }

        RefPtr(RefPtr&& v) noexcept : m_Ptr(v.m_Ptr)
        {
            v.m_Ptr = nullptr;
        }

        ~RefPtr()
        {
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
        }

        RefPtr& operator=(const RefPtr& v) noexcept
        {
            if (this != &v)
            {
                if (m_Ptr)
                {
                    m_Ptr->Release();
                }
                m_Ptr = v.m_Ptr;
                if (m_Ptr)
                {
                    m_Ptr->AddRef();
                }
            }

            return *this;
        }

        RefPtr& operator=(RefPtr&& v) noexcept
        {
            if (this != &v)
            {
                if (m_Ptr)
                {
                    m_Ptr->Release();
                }
                m_Ptr = v.m_Ptr;
                v.m_Ptr = nullptr;
            }

            return *this;
        }

        [[nodiscard]] T* Get() const noexcept
        {
            return m_Ptr;
        }

        [[nodiscard]] T& operator*() const noexcept
        {
            return *m_Ptr;
        }

        [[nodiscard]] T* operator->() const noexcept
        {
            return m_Ptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return m_Ptr != nullptr;
        }
    };
}
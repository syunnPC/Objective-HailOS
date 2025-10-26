#pragma once

#include "InterfaceID.hpp"

namespace Kernel
{
    inline constexpr InterfaceID IID_IKernelObject{ {
        0x28, 0x6B, 0xFB, 0x7C, 0x36, 0x2D, 0x0E, 0xC3, 0xE6, 0x8F, 0x2F, 0x53, 0x83, 0x7E, 0xE2, 0x55
    } };

    class IKernelObject
    {
    public:
        virtual std::size_t AddRef() noexcept = 0;
        virtual void Release() noexcept = 0;
        virtual bool QueryInterface(const InterfaceID& iid, void** out) noexcept = 0;
    protected:
        virtual ~IKernelObject() = default;
    };
}
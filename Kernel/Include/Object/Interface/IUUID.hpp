#pragma once

#include "IKernelObject.hpp"
#include "IStringConvertable.hpp"

namespace Kernel
{
    inline constexpr InterfaceID IID_IUUID{ {
        0x0D, 0x71, 0xC3, 0xB5, 0x87, 0x02, 0xF8, 0x47, 0x9F, 0x88, 0xB0, 0xBC, 0xDE, 0x33, 0x9B, 0x8E
    } };

    class IUUID : public virtual IStringConvertible, public virtual IKernelObject
    {
    public:
        virtual const std::uint8_t* Bytes() const noexcept = 0;
        virtual bool operator==(const IUUID& rhs) const noexcept = 0;
        virtual bool operator!=(const IUUID& rhs) const noexcept = 0;
    protected:
        virtual ~IUUID() override = default;
    };
}